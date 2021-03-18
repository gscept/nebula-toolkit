//------------------------------------------------------------------------------
//  selectiontool.cc
//  (C) 2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "util/array.h"
#include "selectiontool.h"
#include "dynui/im3d/im3d.h"
#include "editor/commandmanager.h"

Util::Array<Editor::Entity> Tools::SelectionTool::selection = {};
static bool isDirty = false;
static Im3d::Mat4 tempTransform;

namespace Tools
{

//------------------------------------------------------------------------------
/**
*/
Util::Array<Editor::Entity> const&
SelectionTool::Selection()
{
    return SelectionTool::selection;
}

//------------------------------------------------------------------------------
/**
*/
void
SelectionTool::RenderGizmo()
{
    /*
    if (selectedEntity == Editor::InvalidEntityId)
		return;
	
	auto entity = Editor::EntityFactory::GetEntity(selectedEntity);
	
	if (!entity.isvalid())
		return;

	if (!entity.IsTransformable())
		return;

	if (!isDirty)
	{
		tempTransform = entity.GetTransform();
	}
	
    bool isTransforming = Im3d::Gizmo("GizmoEntity", tempTransform);
	if (isTransforming)
	{
		isDirty = true;
		Editor::SetLocalTransformMsg::Send(entity.GetEntityGuid(), tempTransform);
	}
	else if(isDirty)
	{
		// User has release gizmo, we can set real transform and add to undo queue
		Ptr<Command::SetTransform> cmd = Command::SetTransform::Create();
		cmd->SetGuid(entity.GetEntityGuid());
		cmd->SetValue(tempTransform);
		Edit::CommandManager::Execute(cmd);
		isTransforming = false;
		isDirty = false;
	}
    */
}

} // namespace Tools
