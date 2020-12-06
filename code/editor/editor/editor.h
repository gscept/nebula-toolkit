#pragma once
//------------------------------------------------------------------------------
/**
    @class  Editor::Editor

    Front end for the Nebula editor

    (C) 2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "util/string.h"

namespace Editor
{

/// Create the editor
void Create();

/// Connect to a backend
bool ConnectToBackend(Util::String const& hostname);

/// Spawn a local backend
void SpawnLocalBackend();

/// Destroy the editor
void Destroy();

} // namespace Editor
