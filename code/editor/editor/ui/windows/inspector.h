#pragma once
//------------------------------------------------------------------------------
/**
    @class  Presentation::Inspector

    Property inspector

    (C) 2021 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "editor/ui/window.h"

namespace Presentation
{

class Inspector : public BaseWindow
{
    __DeclareClass(Inspector)
public:
    Inspector();
    ~Inspector();

    void Update();
    void Run();

private:
    void* tempBuffer;
    SizeT tempBufferSize;
};
__RegisterClass(Inspector)

} // namespace Presentation

