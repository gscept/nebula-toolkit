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
bool
CommandManager::Execute(const Command& command)
{
    // Execute the command and add it to undo list if succeeded
    if (command.Execute())
    {
        ClearRedoList();
        AddUndo(command);
        return true;
    }

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
        }

        // Execute commands in reverse order.
        for (IndexT i = stack.Size() - 1; i >= 0; i--)
        {
            Command command = stack[i];
            if (command.Unexecute())
            {
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
            Command& command = stack[i];
            if (command.Execute())
            {
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
void CommandManager::AddUndo(const Command& command)
{
    if (undoListSize >= undoLevel)
    {
        undoList.RemoveFront();
        undoListSize--;
    }

    if (macroMode)
    {
        undoList.Back().Append(command);
    }
    else
    {
        undoList.AddBack({ command });
    }
    undoListSize++;
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
void CommandManager::AddRedo(const Command& command)
{
    if (macroMode)
    {
        redoList.Back().Append(command);
    }
    else
    {
        redoList.AddBack({ command });
    }
    redoListSize++;
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::ClearUndoList()
{
    undoList.Clear();
    undoListSize = 0;
}

//------------------------------------------------------------------------------
/**
*/
void CommandManager::ClearRedoList()
{
    redoList.Clear();
    redoListSize = 0;
}

} // namespace Edit
