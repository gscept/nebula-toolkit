//------------------------------------------------------------------------------
//  uimanager.cc
//  (C) 2019 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "uimanager.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "windowserver.h"
#include "windows/console.h"

namespace Editor
{

static Ptr<Presentation::WindowServer> windowServer;


namespace UIManager
{

//------------------------------------------------------------------------------
/**
*/
void
OnActivate()
{
    windowServer = Presentation::WindowServer::Create();

    windowServer->RegisterWindow("Presentation::Console", "Console", "Debug");
    
    GraphicsFeature::GraphicsFeatureUnit::Instance()->AddRenderUICallback([]()
    {
        windowServer->RunAll();
    });
}

//------------------------------------------------------------------------------
/**
*/
void
OnDeactivate()
{
    windowServer = nullptr;
}

//------------------------------------------------------------------------------
/**
*/
void
OnEndFrame()
{
    windowServer->Update();
}

//------------------------------------------------------------------------------
/**
*/
Game::ManagerAPI
Create()
{
    Game::ManagerAPI api;
    api.OnActivate = &OnActivate;
    api.OnDeactivate = &OnDeactivate;
    api.OnEndFrame = &OnEndFrame;
    return api;
}

} // namespace UIManager

} // namespace Editor
