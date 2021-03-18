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

using namespace Editor;

namespace Presentation
{
__ImplementClass(Presentation::Inspector, 'InWn', Presentation::BaseWindow);

//------------------------------------------------------------------------------
/**
*/
Inspector::Inspector()
{
	this->tempBufferSize = 64;
    this->tempBuffer = Memory::Alloc(Memory::HeapType::DefaultHeap, this->tempBufferSize);
}

//------------------------------------------------------------------------------
/**
*/
Inspector::~Inspector()
{
    Memory::Free(Memory::HeapType::DefaultHeap, this->tempBuffer);
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

	if (!Game::IsActive(Editor::state.editorWorld, entity))
		return;

	Editor::Editable& edit = Editor::state.editables[entity.index];

	//TODO: This "name part" is obviously retarded. Functionality needs to be implemented too.
    char name[128];
	edit.name.CopyToBuffer(name, 128);

	//Active checkbox
	//if(ImGui::Checkbox("##EntityEnabled", &enabled))
	//{}
	//// Name on the same line as checkbox
	//ImGui::SameLine();

	if (ImGui::InputText("##EntityName", name, 128, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		// TODO
	}

    ImGui::Separator();
	ImGui::NewLine();
	//this->ShowAddPropertyMenu();

	MemDb::TableId const category = Game::GetEntityMapping(Editor::state.editorWorld, entity).category;
	MemDb::Row const row = Game::GetEntityMapping(Editor::state.editorWorld, entity).instance;

    auto const& properties = Game::GetWorldDatabase(Editor::state.editorWorld)->GetTable(category).properties;
    for (auto property : properties)
    {
		SizeT const typeSize = MemDb::TypeRegistry::TypeSize(property);
		void* data = Game::GetInstanceBuffer(Editor::state.editorWorld, category, property);
		data = (byte*)data + (row * typeSize);
		if (typeSize > this->tempBufferSize)
		{
			Memory::Free(Memory::HeapType::DefaultHeap, this->tempBuffer);
			this->tempBufferSize = typeSize;
			this->tempBuffer = Memory::Alloc(Memory::HeapType::DefaultHeap, this->tempBufferSize);
		}
		Memory::Copy(data, this->tempBuffer, typeSize);
		bool commitChange = false;
		Game::PropertyInspection::DrawInspector(property, this->tempBuffer, &commitChange);

		if (commitChange)
		{
			//Edit::SetProperty(entity, property, this->tempBuffer);
		}
		ImGui::Separator();
    }

}

} // namespace Presentation
