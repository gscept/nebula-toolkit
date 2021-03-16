#pragma once
//------------------------------------------------------------------------------
/**
    BaseCommand

    Baseclass for actions.

    (C) 2021 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "util/bitfield.h"

namespace Edit
{

class BaseCommand : public Core::RefCounted
{
    __DeclareClass(BaseCommand)
public:
    /// Constructor. Make sure to set undoable to true if wanted
    BaseCommand(bool undoable = false);
    
    /// Destructor. Remember to free any resources if neccessary!
    virtual ~BaseCommand();

    /// do the command
    virtual bool Execute();

    /// undo the command
    virtual bool Unexecute();

    /// Returns whether this command is undoable.
    bool IsUndoable() const;

protected:
    const bool undoable = false;
};

} // namespace Edit
