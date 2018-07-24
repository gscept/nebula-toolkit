import genutil as util


#------------------------------------------------------------------------------
## Constants
#
PACKED_PER_ATTRIBUTE = 0
PACKED_PER_INSTANCE = 1

#------------------------------------------------------------------------------
##
#
def GetEventEnum(string):
    s = string.lower()
    if s == "beginframe":
        return "ComponentEvent::OnBeginFrame"
    elif s == "render":
        return "ComponentEvent::OnRender"
    elif s == "endframe":
        return "ComponentEvent::OnEndFrame"
    elif s == "renderdebug":
        return "ComponentEvent::OnRenderDebug"
    elif s == "activate":
        return "ComponentEvent::OnActivate"
    elif s == "deactivate":
        return "ComponentEvent::OnDeactivate"
    else:
        util.error('"{}" is not a valid event!'.format(string))

#------------------------------------------------------------------------------
##
#
def ConvertToCamelNotation(attrType):
    T = attrType.lower()
    if (T == "byte"):
        return "Byte"
    elif (T == "ubyte"):
        return "UByte"
    elif (T == "short"):
        return "Short"
    elif (T == "ushort"):
        return "UShort"
    elif (T == "int"):
        return "Int"
    elif (T == "uint"):
        return "UInt"
    elif (T == "float"):
        return "Float"
    elif (T == "long"):
        return "Long"
    elif (T == "ulong"):
        return "ULong"
    elif (T == "double"):
        return "Double"
    elif (T == "bool"):
        return "Bool"
    elif (T == "float4"):
        return "Float4"
    elif (T == "vector"):
        return "Vector"
    elif (T == "point"):
        return "Point"
    elif (T == "matrix44"):
        return "Matrix44"
    elif (T == "string"):
        return "String"
    elif (T == "resource"):
        return "String"
    elif (T == "entity"):
        return "Entity"
    else:
        util.error('"{}" is not a valid type!'.format(T))

#------------------------------------------------------------------------------
##
#
def GetTypeString(attrType):
    T = attrType.lower()
    if (T == "byte"):
        return "char"
    elif (T == "ubyte"):
        return "ubyte"
    elif (T == "short"):
        return "short"
    elif (T == "ushort"):
        return "ushort"
    elif (T == "int"):
        return "int"
    elif (T == "uint"):
        return "uint"
    elif (T == "float"):
        return "float"
    elif (T == "long"):
        return "long"
    elif (T == "ulong"):
        return "ulong"
    elif (T == "double"):
        return "double"
    elif (T == "bool"):
        return "bool"
    elif (T == "float4"):
        return "Math::float4"
    elif (T == "vector"):
        return "Math::vector"
    elif (T == "point"):
        return "Math::point"
    elif (T == "matrix44"):
        return "Math::matrix44"
    elif (T == "string"):
        return "Util::String"
    elif (T == "resource"):
        return "Util::String"
    elif (T == "entity"):
        return "Game::Entity"
    else:
        return attrType

#------------------------------------------------------------------------------
##
#
def AccessModeToClassString(accessMode):
    access = accessMode.lower()
    if (access == "rw"):
        return "Attr::ReadWrite"
    elif (access == "r"):
        return "Attr::ReadOnly"
    else:
        util.error('"{}" is not a valid access mode!'.format(access))

#------------------------------------------------------------------------------
##
#
def GetDataLayout(string):
    if string == "PACKED_PER_ATTRIBUTE":
        return PACKED_PER_ATTRIBUTE
    elif string == "PACKED_PER_INSTANCE":
        return PACKED_PER_INSTANCE