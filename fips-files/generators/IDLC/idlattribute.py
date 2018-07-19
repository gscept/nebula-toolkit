import idltypes as IDLTypes
import genutil as util

#------------------------------------------------------------------------------
##
#
def WriteAttributeHeaderDeclarations(f, document):
    for attributeName, attribute in document["attributes"].iteritems():
        if not "type" in attribute:
            util.error('Attribute type is required. Attribute "{}" does not name a type!'.format(attributeName))
        typeString = IDLTypes.TypeToString(attribute["type"])

        if not "fourcc" in attribute:
            util.error('Attribute FourCC is required. Attribute "{}" does not have a fourcc!'.format(attributeName))
        fourcc = attribute["fourcc"]

        accessMode = "rw"
        if "access" in attribute:
            accessMode = IDLTypes.AccessModeToClassString(attribute["access"])
        
        f.WriteLine('Declare{}({}, {}, {});'.format(typeString, attributeName, fourcc, accessMode))
        