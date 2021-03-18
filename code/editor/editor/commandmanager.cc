//------------------------------------------------------------------------------
//  commandmanager.cc
//  (C) 2018 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "commandmanager.h"
#include "util/list.h"
#include "util/arraystack.h"

namespace Edit
{

static CommandManager::CommandList undoList;
static CommandManager::CommandList redoList;
static SizeT undoLevel = 100;
static SizeT cleanCount;
static SizeT undoListSize;
static SizeT redoListSize;
static bool macroMode = false;

//------------------------------------------------------------------------------
/**
*/
void
CommandManager::Create()
{
    undoList.Clear();
    redoList.Clear();
    undoListSize = 0;
    redoListSize = 0;
    cleanCount = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
CommandManager::Discard()
{
    undoList.Clear();
    redoList.Clear();
    undoListSize = 0;
    redoListSize = 0;
    cleanCount = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
CommandManager::CanUndo()
{
    // Cannot undo while in macro mode.
    return (undoList.Size() > 0) && !macroMode;
}

//------------------------------------------------------------------------------
/**
*/
bool
CommandManager::CanRedo()
{
    // Cannot redo while in macro mode.
    return (redoList.Size() > 0) && !macroMode;
}

//------------------------------------------------------------------------------
/**
*/
SizeT
CommandManager::GetUndoLevel()
{
    return undoLevel;
}

//------------------------------------------------------------------------------
/**
*/
void
CommandManager::SetUndoLevel(SizeT newValue)
{
    undoLevel = newValue;
}

//------------------------------------------------------------------------------
/**
*/
bool
CommandManager::IsDirty()
{
    return (cleanCount != 0);
}

//------------------------------------------------------------------------------
/**
*/
CommandManager::CommandStack
CommandManager::GetLastUndoCommand()
{
    return undoList.Back();
}

//------------------------------------------------------------------------------
/**
*/
CommandManager::CommandStack
CommandManager::GetLastRedoCommand()
{
    return redoList.Back();
}

//------------------------------------------------------------------------------
/**
*/
CommandManager::CommandList const&
CommandManager::GetUndoList()
{
    return undoList;
}

//------------------------------------------------------------------------------
/**
*/
CommandManager::CommandList const&
CommandManager::GetRedoList()
{
    return redoList;
}

//------------------------------------------------------------------------------
/**
*/
bool
CommandManager::Execute(Command* command)
{
    // Execute the command and add it to undo list if succeeded
    if (command->Execute())
    {
        command->executed = true;
        ClearRedoList();
        AddUndo(command);
        return true;
    }

    n_delete(command);
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
CommandManager::BeginMacro()
{
    if (macroMode)
    {
        n_warning("Can only construct one macro at a time!\n");
        return;
    }

    macroMode = true;
    undoList.AddBack({});
    undoListSize++;
}

//------------------------------------------------------------------------------
/**
*/
void
CommandManager::EndMacro()
{
    macroMode = false;
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::Undo()
{
    if (CanUndo())
    {
        cleanCount--;
        CommandStack stack = undoList.RemoveBack();
        undoListSize--;
        
        if (stack.Size() == 0)
            return;
        
        if (stack.Size() > 1)
        {
            // Enable macro mode since the cmd is a macro
            macroMode = true;
            redoList.AddBack({});
            redoListSize++;
        }

        // Execute commands in reverse order.
        for (IndexT i = stack.Size() - 1; i >= 0; i--)
        {
            Command* command = stack[i];
            if (command->Unexecute())
            {
                command->executed = false;
                AddRedo(command);
            }
        }

        macroMode = false;
    }
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::Redo()
{
    if (CanRedo())
    {
        cleanCount++;
        CommandStack stack = redoList.RemoveBack();
        redoListSize--;
        if (stack.Size() == 0)
            return;
        
        if (stack.Size() > 1)
        {
            // Enable macro mode since the cmd is a macro
            BeginMacro();
        }

        // Execute commands in reverse order.
        for (IndexT i = stack.Size() - 1; i >= 0; i--)
        {
            Command* command = stack[i];
            if (command->Execute())
            {
                command->executed = true;
                AddUndo(command);
            }
        }

        EndMacro();
    }
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::Clear()
{
    ClearUndoList();
    ClearRedoList();
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::SetClean()
{
    cleanCount = 0;
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::AddUndo(Command* command)
{
    if (undoListSize >= undoLevel)
    {
        // history limit reached, start popping
        CommandStack stack = undoList.RemoveFront();
        for (Command* cmd : stack)
            n_delete(cmd);
        undoListSize--;
    }

    if (macroMode)
    {
        undoList.Back().Append(command);
    }
    else
    {
        undoList.AddBack({ command });
        undoListSize++;
    }
    if (cleanCount < 0 && redoListSize > 0)
    {
        cleanCount = undoListSize + redoListSize + 1;
    }
    else
    {
        cleanCount++;
    }
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::AddRedo(Command* command)
{
    if (macroMode)
    {
        redoList.Back().Append(command);
    }
    else
    {
        redoList.AddBack({ command });
        redoListSize++;
    }
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::ClearUndoList()
{
    for (CommandList::Iterator it = undoList.Begin(); it != undoList.End(); it++)
    {
        for (Command* cmd : (*it))
            n_delete(cmd);
    }
    undoList.Clear();
    undoListSize = 0;
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::ClearRedoList()
{
    for (CommandList::Iterator it = redoList.Begin(); it != redoList.End(); it++)
    {
        for (Command* cmd : (*it))
            n_delete(cmd);
    }
    redoList.Clear();
    redoListSize = 0;
}

} // namespace Edit
