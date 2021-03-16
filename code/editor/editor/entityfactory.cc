////------------------------------------------------------------------------------
////  @file entityfactory.cc
////  @copyright (C) 2021 Individual contributors, see AUTHORS file
////------------------------------------------------------------------------------
//#include "foundation/stdneb.h"
//#include "util/random.h"
//#include "entityfactory.h"
//
//namespace Editor
//{
//
//__ImplementSingleton(EntityManager)
//
////------------------------------------------------------------------------------
///**
//*/
//EntityManager::EntityManager()
//{
//    // empty
//}
//
////------------------------------------------------------------------------------
///**
//*/
//EntityManager::~EntityManager()
//{
//    // empty
//}
//
////------------------------------------------------------------------------------
///**
//*/
//void
//EntityManager::Create()
//{
//    n_assert(!EntityManager::HasInstance());
//    EntityManager::Singleton = n_new(EntityManager);
//}
//
////------------------------------------------------------------------------------
///**
//*/
//void
//EntityManager::Destroy()
//{
//    n_delete(EntityManager::Singleton);
//    EntityManager::Singleton = nullptr;
//}
//
//
////------------------------------------------------------------------------------
///**
//*/
//void*
//GetValuePointer(Editor::Entity entity, Game::PropertyId pid)
//{
//    Game::EntityMapping mapping = EntityManager::Singleton->editorWorld.entityMap[entity.index];
//    byte* buf = (byte*)EntityManager::Singleton->editorWorld.db->GetBuffer(
//        mapping.category, 
//        EntityManager::Singleton->editorWorld.db->GetColumnId(mapping.category, pid)
//    );
//    return buf + (mapping.instance.id * (uint64_t)MemDb::TypeRegistry::TypeSize(pid));
//}
//
////------------------------------------------------------------------------------
///**
//*/
//Editor::Entity
//EntityManager::CreateEntity(Util::StringAtom templateName)
//{
//    Game::TemplateId const tid = Game::GetTemplateId(templateName);
//
//    if (tid == Game::InvalidTemplateId)
//    {
//        return Editor::Entity::Invalid();
//    }
//
//    Game::EntityCreateInfo createInfo;
//    createInfo.immediate = true;
//    createInfo.templateId = tid;
//    Game::Entity const entity = Game::CreateEntity(createInfo);
//    Math::mat4 t = Math::translation({ Util::RandomFloatNTP() * 10.0f, 0, Util::RandomFloatNTP() * 10.0f });
//    Game::SetProperty(entity, Game::GetPropertyId("WorldTransform"_atm), t);
//
//    Game::Entity editorEntity;
//    if (!Singleton->editorWorld.pool.Allocate(editorEntity))
//    {
//        Singleton->editorWorld.entityMap.Append({});
//        Singleton->editables.Append({});
//    }
//
//    Game::InstanceId const instance = Singleton->editorWorld.AllocateInstance(editorEntity, tid);
//    Editable& edit = Singleton->editables[editorEntity.index];
//    edit.gameEntity = entity;
//
//    Math::mat4& et = *(Math::mat4*)GetValuePointer(editorEntity, Game::GetPropertyId("WorldTransform"_atm));
//    et = t;
//
//    return editorEntity;
//}
//
////------------------------------------------------------------------------------
///**
//*/
//void
//EntityManager::DestroyEntity(Editor::Entity editorEntity)
//{
//    Game::EntityMapping& mapping = Singleton->editorWorld.entityMap[editorEntity.index];
//    Editable& edit = Singleton->editables[editorEntity.index];
//
//    if (Game::IsValid(edit.gameEntity))
//        Game::DeleteEntity(edit.gameEntity);
//
//    edit.gameEntity = Game::Entity::Invalid();
//
//    Singleton->editorWorld.DeallocateInstance(mapping.category, mapping.instance);
//    Singleton->editorWorld.pool.Deallocate(editorEntity);
//
//    // Make sure the editor world is always defragged
//    Singleton->editorWorld.DefragmentCategoryInstances(mapping.category);
//
//    mapping.category = Game::CategoryId::Invalid();
//    mapping.instance = Game::InstanceId::Invalid();
//}
//
//
//} // namespace Editor
