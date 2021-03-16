//------------------------------------------------------------------------------
//  editor.cc
//  (C) 2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "editor.h"

#include "io/assignregistry.h"
#include "scripting/scriptserver.h"
#include "memdb/database.h"
#include "game/api.h"
#include "basegamefeature/basegamefeatureunit.h"
#include "basegamefeature/managers/timemanager.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "imgui.h"
#include "util/random.h"
#include "commandmanager.h"

using namespace Net;

namespace Editor
{

//------------------------------------------------------------------------------
/**
*/
EditorState state;

//------------------------------------------------------------------------------
/**
*/
void*
GetValuePointer(Editor::Entity entity, Game::PropertyId pid)
{
    Game::EntityMapping mapping = state.editorWorld->entityMap[entity.index];
    byte* buf = (byte*)state.editorWorld->db->GetBuffer(mapping.category, state.editorWorld->db->GetColumnId(mapping.category, pid));
    return buf + (mapping.instance * (uint64_t)MemDb::TypeRegistry::TypeSize(pid));
}

//------------------------------------------------------------------------------
/**
*/
Editor::Entity
CreateEntity(EntityGuid guid, Util::StringAtom templateName)
{
    Game::TemplateId const tid = Game::GetTemplateId(templateName);

    if (tid == Game::InvalidTemplateId)
    {
        return Editor::Entity::Invalid();
    }

    Game::World* gameWorld = Game::GetWorld(WORLD_DEFAULT);
    Game::EntityCreateInfo createInfo;
    createInfo.immediate = true;
    createInfo.templateId = tid;
    Game::Entity const entity = Game::CreateEntity(gameWorld, createInfo);
    Math::mat4 t = Math::translation({ Util::RandomFloatNTP() * 10.0f, 0, Util::RandomFloatNTP() * 10.0f });
    Game::SetProperty(gameWorld, entity, Game::GetPropertyId("WorldTransform"_atm), t);

    Editor::Entity editorEntity = Game::AllocateEntity(state.editorWorld);
    if (state.editables.Size() >= editorEntity.index)
        state.editables.Append({});
    Game::AllocateInstance(state.editorWorld, editorEntity, tid);

    Editable& edit = state.editables[editorEntity.index];
    edit.gameEntity = entity;
    edit.guid = guid;
    edit.name = templateName.AsString().ExtractFileName();

    Math::mat4& et = *(Math::mat4*)GetValuePointer(editorEntity, Game::GetPropertyId("WorldTransform"_atm));
    et = t;

    state.guidMap.Add(guid, editorEntity);

    return editorEntity;
}

//------------------------------------------------------------------------------
/**
*/
Editor::Entity
CreateEntity(EntityGuid guid, MemDb::TableId editorTable, Util::Blob entityState)
{
    Game::World* gameWorld = Game::GetWorld(WORLD_DEFAULT);

    MemDb::TableSignature const& signature = state.editorWorld->db->GetTableSignature(editorTable);

    Game::Entity const entity = Game::AllocateEntity(gameWorld);
    MemDb::TableId gameTable = gameWorld->db->FindTable(signature);
    n_assert(gameTable != MemDb::InvalidTableId);
    MemDb::Row instance = Game::AllocateInstance(gameWorld, entity, gameTable);
    gameWorld->db->DeserializeInstance(entityState, gameTable, instance);
    Game::SetProperty<Game::Entity>(gameWorld, entity, Game::GetPropertyId("Owner"_atm), entity);

    Editor::Entity editorEntity = Game::AllocateEntity(state.editorWorld);
    if (state.editables.Size() >= editorEntity.index)
        state.editables.Append({});
    MemDb::Row editorInstance = Game::AllocateInstance(state.editorWorld, editorEntity, editorTable);
    state.editorWorld->db->DeserializeInstance(entityState, editorTable, editorInstance);
    Game::SetProperty<Editor::Entity>(state.editorWorld, editorEntity, Game::GetPropertyId("Owner"_atm), editorEntity);

    Editable& edit = state.editables[editorEntity.index];
    edit.gameEntity = entity;
    edit.guid = guid;

    state.guidMap.Add(guid, editorEntity);

    return editorEntity;
}

//------------------------------------------------------------------------------
/**
*/
void
DestroyEntity(Editor::EntityGuid entityGuid)
{
    IndexT const idx = state.guidMap.FindIndex(entityGuid);
    if (idx == InvalidIndex)
        return;

    Editor::Entity const editorEntity = state.guidMap.ValueAtIndex(idx);
    Game::EntityMapping mapping = Game::GetEntityMapping(state.editorWorld, editorEntity);
    Editable& edit = state.editables[editorEntity.index];

    if (Game::IsValid(Game::GetWorld(WORLD_DEFAULT), edit.gameEntity))
        Game::DeleteEntity(Game::GetWorld(WORLD_DEFAULT), edit.gameEntity);

    edit.gameEntity = Game::Entity::Invalid();

    Game::DeallocateInstance(state.editorWorld, editorEntity);
    Game::DeallocateEntity(state.editorWorld, editorEntity);

    // Make sure the editor world is always defragged
    Game::Defragment(state.editorWorld, mapping.category);

    state.guidMap.Erase(entityGuid);
}

//------------------------------------------------------------------------------
/**
*/
bool
SetProperty(Editor::EntityGuid guid, Game::PropertyId pid, void* value, size_t size)
{
    IndexT const idx = state.guidMap.FindIndex(guid);
    if (idx == InvalidIndex)
        false;

    Editor::Entity const editorEntity = state.guidMap.ValueAtIndex(idx);
    Game::EntityMapping mapping = Game::GetEntityMapping(state.editorWorld, editorEntity);
    Editable& edit = state.editables[editorEntity.index];

    Game::SetProperty(state.editorWorld, editorEntity, pid, value, size);

    if (Game::IsValid(Game::GetWorld(WORLD_DEFAULT), edit.gameEntity))
        Game::SetProperty(Game::GetWorld(WORLD_DEFAULT), edit.gameEntity, pid, value, size);

    return true;
}

//------------------------------------------------------------------------------
/**
*/
EntityGuid
CmdCreateEntity(Util::StringAtom templateName)
{
    EntityGuid guid;
    guid.Generate();

    Edit::Command cmd;
    cmd.name = "Create Entity";
    cmd.Execute = [guid, templateName]()
    {
        return (CreateEntity(guid, templateName) != Editor::Entity::Invalid());
    };
    cmd.Unexecute = [guid]()
    {
        DestroyEntity(guid);
        return true;
    };

    if (!Edit::CommandManager::Execute(cmd))
    {
        return EntityGuid();
    }

    return guid;
}

//------------------------------------------------------------------------------
/**
*/
void
CmdDestroyEntity(Editor::EntityGuid guid)
{
    Edit::Command cmd;
    cmd.name = "Create Entity";
    cmd.Execute = [guid]()
    {
        DestroyEntity(guid);
        return true;
    };

    Editor::Entity e = state.guidMap[guid];
    Game::EntityMapping mapping = Game::GetEntityMapping(state.editorWorld, e);
    Util::Blob entityState = state.editorWorld->db->SerializeInstance(mapping.category, mapping.instance);
    MemDb::TableId tid = mapping.category;

    cmd.Unexecute = [guid, tid, entityState]()
    {
        CreateEntity(guid, tid, entityState);
        return true;
    };

    Edit::CommandManager::Execute(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
CmdSetPropertyValue(EntityGuid entity, Game::PropertyId pid, void* value, size_t size)
{
    Edit::Command cmd;
    cmd.name = "Set property value";
    Util::Blob newData = Util::Blob(value, size);
    cmd.Execute = [entity, pid, newData]()
    {
        SetProperty(entity, pid, newData.GetPtr(), newData.Size());
        return true;
    };

    Editor::Entity e = state.guidMap[entity];
    Game::EntityMapping mapping = Game::GetEntityMapping(state.editorWorld, e);
    MemDb::TableId tid = mapping.category;
    void* oldValuePtr = state.editorWorld->db->GetValuePointer(tid, state.editorWorld->db->GetColumnId(tid, pid), mapping.instance);
    
    Util::Blob oldData = Util::Blob(oldValuePtr, size);
    cmd.Unexecute = [entity, pid, oldData]()
    {
        SetProperty(entity, pid, oldData.GetPtr(), oldData.Size());
        return true;
    };

    Edit::CommandManager::Execute(cmd);
}

//------------------------------------------------------------------------------
/**
*/
void
Create()
{
    IO::AssignRegistry::Instance()->SetAssign(IO::Assign("edscr", "bin:editorscripts"));

    Scripting::ScriptServer::Instance()->AddModulePath("edscr:");
    Scripting::ScriptServer::Instance()->EvalFile("edscr:bootstrap.py");

    /// Import reload to be able to reload modules.
    Scripting::ScriptServer::Instance()->Eval("from importlib import reload");

    Game::TimeManager::SetGlobalTimeFactor(0.0f);

    Game::WorldCreateInfo worldInfo;
    worldInfo.hash = WORLD_EDITOR;
    state.editorWorld = Game::CreateWorld(worldInfo);

    Game::GameServer::Instance()->SetupEmptyWorld(state.editorWorld);
}

//------------------------------------------------------------------------------
/**
*/
bool
ConnectToBackend(Util::String const& hostname)
{
    if (!state.client.isvalid())
        state.client = TcpClient::Create();

    state.client->SetServerAddress(IpAddress(hostname, 2102));
    TcpClient::Result res = state.client->Connect();

    if (res == TcpClient::Connecting)
    {
        // FIXME: should move this to a separate thread
        Core::SysFunc::Sleep(0.1);
    };

    if (state.client->IsConnected())
    {
        n_printf("Successfully connected to backend!\n");
        return true;
    }

    n_warning("Could not open EditorFeatureUnit's TcpServer!\n");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
SpawnLocalBackend()
{
    n_error("FIXME");
}

//------------------------------------------------------------------------------
/**
*/
void
Destroy()
{
    if (state.client.isvalid() && state.client->IsConnected())
        state.client->Disconnect();
}

//------------------------------------------------------------------------------
/**
*/
void
PlayGame()
{
    if (!state.isPlayingGame)
    {
        
    }
    Game::TimeManager::SetGlobalTimeFactor(1.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
PauseGame()
{
    Game::TimeManager::SetGlobalTimeFactor(0.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
SetTimeScale(float timeScale)
{
    if (state.isPlayingGame)
    {
        Game::TimeManager::SetGlobalTimeFactor(timeScale);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
StopGame()
{
    Game::World* gameWorld = Game::GetWorld(WORLD_DEFAULT);
    Game::GameServer::Instance()->CleanupWorld(gameWorld);
    Game::GameServer::Instance()->SetupEmptyWorld(gameWorld);
    
    gameWorld->categoryDecayMap = state.editorWorld->categoryDecayMap;
    gameWorld->blueprintCatMap = state.editorWorld->blueprintCatMap;
    gameWorld->entityMap = state.editorWorld->entityMap;
    gameWorld->numEntities = state.editorWorld->numEntities;
    gameWorld->pool = state.editorWorld->pool;
    gameWorld->db = nullptr;
    gameWorld->db = MemDb::Database::Create();
    
    state.editorWorld->db->Copy(gameWorld->db);
    
    Game::PrefilterProcessors(gameWorld);

    // update the editables so that they point to the correct game entities.
    Game::FilterCreateInfo filterInfo;
    filterInfo.inclusive[0] = Game::GetPropertyId("Owner"_atm);
    filterInfo.access[0] = Game::AccessMode::READ;
    filterInfo.numInclusive = 1;

    Game::Filter filter = Game::CreateFilter(filterInfo);
    Game::Dataset data = Game::Query(state.editorWorld, filter);

    for (int v = 0; v < data.numViews; v++)
    {
        Game::Dataset::CategoryTableView const& view = data.views[v];
        Editor::Entity const* const entities = (Editor::Entity*)view.buffers[0];

        for (IndexT i = 0; i < view.numInstances; ++i)
        {
            Editor::Entity const& editorEntity = entities[i];
            Editable& edit = state.editables[editorEntity.index];
            // NOTE: assumes the game entity id will be the same as the editor entity id when we've just copied the world.
            edit.gameEntity = editorEntity;
        }
    }

    Game::DestroyFilter(filter);

    Game::TimeManager::SetGlobalTimeFactor(0.0f);
}


} // namespace Editor

