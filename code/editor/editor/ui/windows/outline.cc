//------------------------------------------------------------------------------
//  outline.cc
//  (C) 2021 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "outline.h"
#include "editor/editor.h"
#include "editor/commandmanager.h"
#include "editor/ui/uimanager.h"

using namespace Editor;

namespace Presentation
{
__ImplementClass(Presentation::Outline, 'OtWn', Presentation::BaseWindow);

//------------------------------------------------------------------------------
/**
*/
Outline::Outline()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Outline::~Outline()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Outline::Run()
{
    static bool renaming = false;
    const char* filterPopup = "outline_filter_popup";
    static bool filtering = false;
    static ImGuiTextFilter nameFilter;
    static ImGuiTextFilter guidFilter;
    static ImGuiTextFilter tagFilter;
    static ImGuiTextFilter blueprintFilter;
    static float filterDistance = 0.0f;

    if (ImGui::Button("Filter"))
    {
        ImGui::OpenPopup(filterPopup);
    }
    ImGui::SameLine();
    nameFilter.Draw("Search", 180);
    if (filtering) 
    {
        ImGui::SameLine();
        if (ImGui::Button("Reset"))
        {
            nameFilter.Clear();
            guidFilter.Clear();
            tagFilter.Clear();
            blueprintFilter.Clear();
            filterDistance = 0.0f;
        }
    }

    // always reset filtering status
    filtering = false;
    // set filtering to true if any filter is active
    filtering |= nameFilter.IsActive();
    filtering |= guidFilter.IsActive();
    filtering |= tagFilter.IsActive();
    filtering |= blueprintFilter.IsActive();
    filtering |= filterDistance != 0.0f;
    
    ImVec2 windowPos = ImGui::GetWindowPos();
    float windowWidth = ImGui::GetWindowWidth();
    if (ImGui::BeginPopup(filterPopup))
    {
        ImGui::SetWindowPos({windowPos.x + windowWidth, windowPos.y});
        ImGui::Text("Filter outline");
        ImGui::Separator();
        nameFilter.Draw("Name", 180);
        guidFilter.Draw("Guid", 180);
        tagFilter.Draw("Tag", 180);
        blueprintFilter.Draw("Blueprint", 180);

        ImGui::Separator();
        ImGui::SliderFloat("Distance", &filterDistance, 0.0f, 1000.0f);
        ImGui::SameLine();
        ImGui::TextDisabled(" -Disabled");
        // TODO: Filter by property, spatially, etc.
        ImGui::EndPopup();
    }

    ImGui::Separator();

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
    {
        Game::FilterCreateInfo filterInfo;
        filterInfo.inclusive[0] = Game::GetPropertyId("Owner"_atm);
        filterInfo.access   [0] = Game::AccessMode::READ;
        filterInfo.numInclusive = 1;

        Game::Filter filter = Game::CreateFilter(filterInfo);
        Game::Dataset data = Game::Query(state.editorWorld, filter);
        
        for (int v = 0; v < data.numViews; v++)
        {
            Game::Dataset::CategoryTableView const& view = data.views[v];
            Editor::Entity const* const entities = (Editor::Entity*)view.buffers[0];
            
            for (IndexT i = 0; i < view.numInstances; ++i)
            {
                Editor::Entity const& entity = entities[i];
                Editable& edit = state.editables[entity.index];

                if (!Game::IsValid(state.editorWorld, entity))
                    continue;

                if (!nameFilter.PassFilter(edit.name.AsCharPtr()))
                    continue;
                if (!guidFilter.PassFilter(edit.guid.AsString().AsCharPtr()))
                    continue;
                
                ImGui::BeginGroup();
                {
                    ImGui::TextDisabled("|-");
                    ImGui::SameLine();
                    ImGui::Image(&UIManager::Icons::game, {20,20});
                    ImGui::SameLine();
                    ImGui::Selectable(
                        edit.name.AsCharPtr(),
                        state.selected == entity,
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_AllowDoubleClick |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_SpanAllColumns |
                        ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups
                    );
                    ImGui::SameLine();
                    ImGui::TextColored({ 0,0,0.6f,1 }, state.editables[entity.index].guid.AsString().AsCharPtr());

                    if (ImGui::IsItemClicked(0) || ImGui::IsItemClicked(1))
                    {
                        state.selected = entity;
                    }
                    if (state.selected == entity && ImGui::BeginPopupContextWindow())
                    {
                        if (ImGui::Selectable("Copy"))
                        {
                            // TODO: implement
                        }
                        if (ImGui::Selectable("Paste"))
                        {
                            // TODO: implement
                        }
                        ImGui::Separator();
                        if (ImGui::Selectable("Duplicate"))
                        {
                            // TODO: implement
                            // Editor::SceneTree::Instance()->DuplicateEntity(hNode.entity);
                        }
                        if (ImGui::Selectable("New child"))
                        {
                            // TODO: Implement
                        }
                        ImGui::Separator();
                        if (ImGui::Selectable("Delete"))
                        {
                            CmdDestroyEntity(state.editables[entity.index].guid);
                            state.selected = Editor::Entity::Invalid();
                        }

                        ImGui::EndPopup();
                    }
                }
                ImGui::EndGroup();
            }
        }

        Game::DestroyFilter(filter);
    }
    ImGui::EndChild();
}

} // namespace Presentation
