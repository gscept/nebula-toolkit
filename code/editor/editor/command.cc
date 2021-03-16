//------------------------------------------------------------------------------
//  command.cc
//  (C) 2018-2019 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"

#include "command.h"
namespace Edit
{
__ImplementClass(Edit::BaseCommand, 'ECMD', Core::RefCounted)

//------------------------------------------------------------------------------
/**
*/
BaseCommand::BaseCommand(bool undoable) :
    undoable(undoable)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
BaseCommand::~BaseCommand()
{
    // empty, override in subclass
}

//------------------------------------------------------------------------------
/**
*/
bool
BaseCommand::Execute()
{
    // empty, override in subclass
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
BaseCommand::Unexecute()
{
    // empty, override in subclass if needed.
    return false;
}

//------------------------------------------------------------------------------
/**
*/
bool
BaseCommand::IsUndoable() const
{
    return this->undoable;
}

} // namespace Edit
