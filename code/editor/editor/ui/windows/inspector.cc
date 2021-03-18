//------------------------------------------------------------------------------
//  inspector.cc
//  (C) 2021 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "inspector.h"
#include "editor/editor.h"
#include "editor/commandmanager.h"
#include "editor/ui/uimanager.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "editor/tools/selectiontool.h"
#include "game/propertyinspection.h"
#include "editor/cmds.h"

using namespace Editor;

namespace Presentation
{
__ImplementClass(Presentation::Inspector, 'InWn', Presentation::BaseWindow);

//------------------------------------------------------------------------------
/**
*/
Inspector::Inspector()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Inspector::~Inspector()
{
	for (auto const& p : this->tempProperties)
	{
		if (p.buffer != nullptr)
    		Memory::Free(Memory::HeapType::DefaultHeap, p.buffer);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Inspector::Update()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Inspector::Run()
{
	auto const& selection = Tools::SelectionTool::Selection();
	
	if (selection.Size() != 1)
		return;
	
	Editor::Entity const entity = selection[0];

	if (!Game::IsValid(Editor::state.editorWorld, entity) || !Game::IsActive(Editor::state.editorWorld, entity))
		return;

	Editor::Editable& edit = Editor::state.editables[entity.index];

    static char name[128];
	if (this->latestInspectedEntity != entity)
	{
		edit.name.CopyToBuffer(name, 128);
		for (auto& p : this->tempProperties)
			p.isDirty = false; // reset dirty status if we switch entity
		this->latestInspectedEntity = entity;
	}
	
	if (ImGui::InputText("##EntityName", name, 128, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		Edit::SetEntityName(entity, name);
	}

    ImGui::Separator();
	ImGui::NewLine();
	//this->ShowAddPropertyMenu();

	MemDb::TableId const category = Game::GetEntityMapping(Editor::state.editorWorld, entity).category;
	MemDb::Row const row = Game::GetEntityMapping(Editor::state.editorWorld, entity).instance;

	auto const& properties = Game::GetWorldDatabase(Editor::state.editorWorld)->GetTable(category).properties;
	while (this->tempProperties.Size() < properties.Size())
		this->tempProperties.Append({}); // fill up with empty intermediates

    for (int i = 0; i < properties.Size(); i++)
    {
		auto property = properties[i];
		auto& tempProperty = this->tempProperties[i];
		SizeT const typeSize = MemDb::TypeRegistry::TypeSize(property);
		void* data = Game::GetInstanceBuffer(Editor::state.editorWorld, category, property);
		data = (byte*)data + (row * typeSize);
		if (typeSize > tempProperty.bufferSize)
		{
			if (tempProperty.buffer != nullptr)
				Memory::Free(Memory::HeapType::DefaultHeap, tempProperty.buffer);
			tempProperty.bufferSize = typeSize;
			tempProperty.buffer = Memory::Alloc(Memory::HeapType::DefaultHeap, tempProperty.bufferSize);
		}
		if (!tempProperty.isDirty)
		{
			Memory::Copy(data, tempProperty.buffer, typeSize);
			tempProperty.isDirty = true;
		}

		bool commitChange = false;
		Game::PropertyInspection::DrawInspector(property, tempProperty.buffer, &commitChange);

		if (commitChange)
		{
			Edit::SetProperty(entity, property, tempProperty.buffer);
			tempProperty.isDirty = false;
		}
		ImGui::Separator();
    }

}

} // namespace Presentation
