//#pragma once
////------------------------------------------------------------------------------
///**
//    @class Editor::EntityFactory
//
//    @copyright
//    (C) 2021 Individual contributors, see AUTHORS file
//*/
////------------------------------------------------------------------------------
//#include "core/singleton.h"
//#include "game/category.h"
//#include "game/gameserver.h"
//#include "game/entitypool.h"
//#include "util/guid.h"
//
//namespace Editor
//{
//
////------------------------------------------------------------------------------
///**
//*/
//typedef Game::Entity Entity;
//typedef Util::Guid EntityGuid;
//
////------------------------------------------------------------------------------
///**
//*/
//struct Editable
//{
//    /// which game entity in the game database the property is associated with
//    Game::Entity gameEntity = Game::Entity::Invalid();
//};
//
////------------------------------------------------------------------------------
///**
//*/
//class EntityManager
//{
//    __DeclareSingleton(EntityManager);
//public:
//    /// create the singleton
//    static void Create();
//
//    /// destroy the singleton
//    static void Destroy();
//
//    /// Create an entity. Name can be blueprint or template name
//    static Editor::EntityGuid CreateEntity(Util::StringAtom templateName);
//
//    /// Delete an entity
//    static void DestroyEntity(Editor::EntityGuid entity);
//
//private:
//    /// constructor
//    EntityManager();
//    /// destructor
//    ~EntityManager();
//
//public:
//    ///
//    Util::Array<Editable> editables;
//    /// contains the world state for the editor
//    Game::World editorWorld;
//};
//
//} // namespace Editor
