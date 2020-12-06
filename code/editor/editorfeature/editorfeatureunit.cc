//------------------------------------------------------------------------------
//  editorfeatureunit.cc
//  (C) 2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "editorfeatureunit.h"
#include "basegamefeature/managers/entitymanager.h"
#include "editor/editor.h"
#include "editor/ui/uimanager.h"

namespace EditorFeature
{

__ImplementClass(EditorFeature::EditorFeatureUnit, 'edfu', Game::FeatureUnit);
__ImplementSingleton(EditorFeature::EditorFeatureUnit);

//------------------------------------------------------------------------------
/**
*/
EditorFeatureUnit::EditorFeatureUnit()
{
	__ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
EditorFeatureUnit::~EditorFeatureUnit()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
EditorFeatureUnit::OnActivate()
{
    FeatureUnit::OnActivate();
    if (this->args.GetBoolFlag("-editor"))
    {
        Editor::Create();
        // Editor::ConnectToBackend( ... );

        this->AttachManager(Editor::UIManager::Create());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EditorFeatureUnit::OnDeactivate()
{
    FeatureUnit::OnDeactivate();
    if (this->args.GetBoolFlag("-editor"))
    {
        Editor::Destroy();
    }
}

//------------------------------------------------------------------------------
/**
*/
void
EditorFeatureUnit::OnEndFrame()
{
    FeatureUnit::OnEndFrame();
}

//------------------------------------------------------------------------------
/**
*/
void
EditorFeatureUnit::OnRenderDebug()
{
    FeatureUnit::OnRenderDebug();
}

//------------------------------------------------------------------------------
/**
*/
void
EditorFeatureUnit::OnFrame()
{
    FeatureUnit::OnFrame();
}

} // namespace EditorFeature
