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

