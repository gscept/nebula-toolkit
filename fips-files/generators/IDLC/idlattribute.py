import idltypes as IDLTypes
import genutil as util

def Capitalize(s):
    return s[:1].upper() + s[1:]

#------------------------------------------------------------------------------
##
#
def WriteAttributeHeaderDeclarations(f, document):
    for attributeName, attribute in document["attributes"].iteritems():
        if not "type" in attribute:
            util.error('Attribute type is required. Attribute "{}" does not name a type!'.format(attributeName))
        typeString = IDLTypes.ConvertToCamelNotation(attribute["type"])

        if not "fourcc" in attribute:
            util.error('Attribute FourCC is required. Attribute "{}" does not have a fourcc!'.format(attributeName))
        fourcc = attribute["fourcc"]

        accessMode = "rw"
        if "access" in attribute:
            accessMode = IDLTypes.AccessModeToClassString(attribute["access"])
        
        f.WriteLine('Declare{}({}, \'{}\', {});'.format(typeString, attributeName, fourcc, accessMode))

#------------------------------------------------------------------------------
##
#
def WriteEnumeratedTypes(f, document):
    if "enums" in document:
        for enumName, enum in document["enums"].iteritems():
            # Declare Enums
            f.InsertNebulaDivider()
            f.WriteLine("enum {}".format(enumName))
            f.WriteLine("{")
            f.IncreaseIndent()
            numValues = 0
            for key, value in enum.iteritems():
                f.WriteLine("{} = {},".format(key, value))
                numValues += 1
            f.WriteLine("Num{} = {}".format(Capitalize(enumName), numValues))
            f.DecreaseIndent()
            f.WriteLine("};")
            f.WriteLine("")