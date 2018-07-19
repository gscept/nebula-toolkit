DEFAULT_NAMESPACE = "Components"

#------------------------------------------------------------------------------
## Write header for include file
#
def WriteIncludeHeader(f):
    f.WriteLine("#pragma once")
    f.WriteLine("//------------------------------------------------------------------------------")
    f.WriteLine("/**")
    f.IncreaseIndent()
    f.WriteLine("This file was generated with Nebula's IDL compiler tool.")
    f.WriteLine("DO NOT EDIT")
    f.DecreaseIndent()
    f.WriteLine("*/")

#------------------------------------------------------------------------------
## parse and write include statements to file
#
def WriteIncludes(f, document):
    for include in document["includes"]:
        f.Write("#include \"")
        f.Write(include)
        f.Write("\"\n")
    # todo: Dependencies

#------------------------------------------------------------------------------
## Write attribute library declarations to file.
#
def WriteAttributeLibraryDeclaration(f):
    f.WriteLine("#include \"game/attr/attrid.h\"")
    f.WriteLine("")

#------------------------------------------------------------------------------
##
#
def BeginNamespace(f, document):
    f.WriteLine("//------------------------------------------------------------------------------")
    f.Write("namespace ")
    if "namespace" in document:
        f.Write(document["namespace"])
    else:
        f.Write(DEFAULT_NAMESPACE)
    f.WriteLine("")
    f.WriteLine("{")
    f.IncreaseIndent()

#------------------------------------------------------------------------------
##
#
def EndNamespace(f, document):
    f.DecreaseIndent()
    f.Write("} // namespace ")
    if "namespace" in document:
        f.Write(document["namespace"])
    else:
        f.Write(DEFAULT_NAMESPACE)
    f.Write("\n")
    f.WriteLine("//------------------------------------------------------------------------------")