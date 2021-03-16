#pragma once
//------------------------------------------------------------------------------
/**
    @class  Editor::Editor

    Front end for the Nebula editor

    (C) 2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "util/string.h"
#include "util/guid.h"
#include "game/category.h"
#include "game/gameserver.h"
#include "net/tcpclient.h"
#include "net/messageclientconnection.h"

namespace Editor
{

constexpr uint32_t WORLD_EDITOR = uint32_t('EWLD');

typedef Util::Guid EntityGuid;

typedef Game::Entity Entity;

struct Editable
{
    Util::Guid guid;
    Util::String name;
    /// which game entity in the game database the editable is associated with
    Game::Entity gameEntity = Game::Entity::Invalid();
};

struct EditorState
{
    /// TCP network connection
    Ptr<Net::TcpClient> client;
    /// is set to true if the game is currently playing
    bool isPlayingGame = false;
    /// contains the world state for the editor
    Game::World* editorWorld;
    /// @temp
    Editor::Entity selected = Editor::Entity::Invalid();
    /// maps from editor entity index to editable
    Util::Array<Editable> editables;
    /// maps guid to entityid
    Util::Dictionary<EntityGuid, Editor::Entity> guidMap;
};

/// Create the editor
void Create();

/// Connect to a backend
bool ConnectToBackend(Util::String const& hostname);

/// Spawn a local backend
void SpawnLocalBackend();

/// Destroy the editor
void Destroy();

/// Start playing the game.
void PlayGame();

/// Pause the game.
void PauseGame();

/// Stop the game
void StopGame();

/// Create an entity. Name can be blueprint or template name
EntityGuid CmdCreateEntity(Util::StringAtom templateName);

/// Delete an entity
void CmdDestroyEntity(EntityGuid entity);

/// set the value of a property
void CmdSetPropertyValue(EntityGuid entity, Game::PropertyId pid, void* value, size_t size);

/// global editor state
extern EditorState state;

} // namespace Editor
