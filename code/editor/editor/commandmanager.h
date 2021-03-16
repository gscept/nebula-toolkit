#pragma once
//------------------------------------------------------------------------------
/**
    CommandManager

    Implements a undo/redo stack manager.
  
    @todo   Implement execute for and entire CommandStack, so that we can construct
            macros and reuse them

    (C) 2018 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "core/singleton.h"
#include "core/ptr.h"
#include "command.h"

namespace Edit
{

struct Command
{
    const char* name;
    std::function<bool()> Execute;
    std::function<bool()> Unexecute;
};

struct CommandManager
{
    typedef Util::ArrayStack<Command, 1> CommandStack;
    typedef Util::List<CommandStack> CommandList;

    /// Init singleton
    static void Create();
    /// Destroy singleton
    static void Discard();
    /// Returns true if the undo stack has entries
    static bool CanUndo();
    /// Returns true if the redo stack has entries
    static bool CanRedo();
    /// Returns the max amount undos the stack fits
    static SizeT GetUndoLevel();
    /// Set the max amount of undos the stack can fit
    static void SetUndoLevel(SizeT newValue);
    /// Checks if dirty
    static bool IsDirty();
    /// Peeks at the last undo command
    static CommandStack GetLastUndoCommand();
    /// Peeks at the last redo command
    static CommandStack GetLastRedoCommand();
    /// Execute command and add to stack
    static bool Execute(const Command& command);
    /// Begin macro. Subsequent commands will be bundled as one undo list entry
    static void BeginMacro();
    /// End macro.
    static void EndMacro();
    /// Undo last command
    static void Undo();
    /// Redo last command
    static void Redo();
    /// Clear both redo and undo stacks
    static void Clear();
    /// Sets the command manager to a clean state
    static void SetClean();
    /// Adds a command to the undo stack. Does not execute it.
    static void AddUndo(const Command& command);
    /// Adds a command to the redo stack. Does not execute it.
    static void AddRedo(const Command& command);
    /// Clear the undo list
    static void ClearUndoList();
    /// Clear the redo list
    static void ClearRedoList();
};

} // namespace Edit
