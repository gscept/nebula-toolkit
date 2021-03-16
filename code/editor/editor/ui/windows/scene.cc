//------------------------------------------------------------------------------
//  scene.cc
//  (C) 2021 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "scene.h"
#include "editor/editor.h"
#include "editor/commandmanager.h"
#include "editor/ui/uimanager.h"
#include "graphicsfeature/graphicsfeatureunit.h"

using namespace Editor;

namespace Presentation
{
__ImplementClass(Presentation::Scene, 'SCWn', Presentation::BaseWindow);

//------------------------------------------------------------------------------
/**
*/
Scene::Scene()
{
    viewPort.Init(GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView());
    viewPort.SetStage(GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage());
    viewPort.SetFrameBuffer("LightBuffer");
}

//------------------------------------------------------------------------------
/**
*/
Scene::~Scene()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
Scene::Update()
{
    viewPort.Update();
}

//------------------------------------------------------------------------------
/**
*/
void
Scene::Run()
{
   viewPort.Render();
}

} // namespace Presentation
