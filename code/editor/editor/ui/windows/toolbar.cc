//------------------------------------------------------------------------------
//  toolbar.cc
//  (C) 2021 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "toolbar.h"
#include "editor/editor.h"
#include "editor/commandmanager.h"
#include "editor/ui/uimanager.h"
#include "editor/cmds.h"

using namespace Editor;

namespace Presentation
{
__ImplementClass(Presentation::Toolbar, 'TBWn', Presentation::BaseWindow);

//------------------------------------------------------------------------------
/**
*/
Toolbar::Toolbar()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Toolbar::~Toolbar()
{
    // empty
}

#define IMGUI_VERTICAL_SEPARATOR \
    ImGui::SameLine();\
    ImGui::Text(" | "); \
    ImGui::SameLine();

//------------------------------------------------------------------------------
/**
*/
void
Toolbar::Run()
{
    const ImVec2 buttonSize = {32,32};

    if (ImGui::Button("Undo")) { Edit::CommandManager::Undo(); }
    ImGui::SameLine();
    if (ImGui::Button("Redo")) { Edit::CommandManager::Redo(); }
    
    IMGUI_VERTICAL_SEPARATOR;
    
    if (ImGui::ImageButton(&UIManager::Icons::game, buttonSize, {0,0}, {1,1}, 0, ImVec4(0,0,0,0), ImVec4(1,1,1,1)))
    {
        Edit::CreateEntity("MovingEntity/cube");
    }
    
    IMGUI_VERTICAL_SEPARATOR;
    
    if (ImGui::ImageButton(&UIManager::Icons::play, buttonSize, {0,0}, {1,1}, 0)) { PlayGame(); }
    ImGui::SameLine();
    if (ImGui::ImageButton(&UIManager::Icons::pause, buttonSize, {0,0}, {1,1}, 0)) { PauseGame(); }
    ImGui::SameLine();
    if (ImGui::ImageButton(&UIManager::Icons::stop, buttonSize, {0,0}, {1,1}, 0)) { StopGame(); }
}

} // namespace Presentation
