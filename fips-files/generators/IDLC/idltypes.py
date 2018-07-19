import genutil as util

#------------------------------------------------------------------------------
##
#
def TypeToString(attrType):
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
def AccessModeToClassString(accessMode):
    access = accessMode.lower()
    if (access == "rw"):
        return "Attr::ReadWrite"
    elif (access == "r"):
        return "Attr::ReadOnly"
    else:
        util.error('"{}" is not a valid access mode!'.format(access))