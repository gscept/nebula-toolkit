//------------------------------------------------------------------------------
//  editor.cc
//  (C) 2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "editor.h"
#include "net/tcpclient.h"
#include "net/messageclientconnection.h"
#include "io/assignregistry.h"
#include "scripting/scriptserver.h"

using namespace Net;

namespace Editor
{

//------------------------------------------------------------------------------
/**
*/
static
struct EditorState
{
    /// TCP network connection
    Ptr<Net::TcpClient> client;

} state;

//------------------------------------------------------------------------------
/**
*/
void
Create()
{
    //IO::AssignRegistry::Instance()->SetAssign(IO::Assign("edscr", "toolkit:data/scripts"));
    //Scripting::ScriptServer::Instance()->AddModulePath("edscr:");
    //Scripting::ScriptServer::Instance()->EvalFile("edscr:bootstrap.py");

    /// Import reload to be able to reload modules.
    Scripting::ScriptServer::Instance()->Eval("from importlib import reload");
}

//------------------------------------------------------------------------------
/**
*/
bool
ConnectToBackend(Util::String const& hostname)
{
    if (!state.client.isvalid())
        state.client = TcpClient::Create();

    state.client->SetServerAddress(IpAddress(hostname, 2102));
    TcpClient::Result res = state.client->Connect();

    while (res == TcpClient::Connecting)
    {
        // FIXME: should move this to a separate thread
        Core::SysFunc::Sleep(0.1);
    };

    if (res == TcpClient::Success)
    {
        n_printf("Successfully connected to backend!\n");
        return true;
    }

    n_warning("Could not open EditorFeatureUnit's TcpServer!\n");
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
SpawnLocalBackend()
{
    n_error("FIXME");
}

//------------------------------------------------------------------------------
/**
*/
void
Destroy()
{
    if (state.client.isvalid() && state.client->IsConnected())
        state.client->Disconnect();
}

} // namespace Editor

