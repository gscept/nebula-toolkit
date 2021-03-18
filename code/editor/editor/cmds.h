#pragma once
//------------------------------------------------------------------------------
/**
    @file cmds.h

    contains the api for all undo/redo commands

    (C) 2021 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "editor.h"

namespace Edit
{

Editor::Entity CreateEntity(Util::StringAtom templateName);
void DeleteEntity(Editor::Entity entity);
void SetSelection(Util::Array<Editor::Entity> const& selection);

} // namespace Edit
