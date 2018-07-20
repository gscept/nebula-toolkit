default_namespace = "Components"

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
        f.Write(default_namespace)
    f.WriteLine("")
    f.WriteLine("{")

#------------------------------------------------------------------------------
##
#
def EndNamespace(f, document):
    f.Write("} // namespace ")
    if "namespace" in document:
        f.Write(document["namespace"])
    else:
        f.Write(default_namespace)
    f.Write("\n")
    f.WriteLine("//------------------------------------------------------------------------------")

#------------------------------------------------------------------------------
##
#
def BeginNamespaceOverride(f, document, namespace):
    f.WriteLine("//------------------------------------------------------------------------------")
    f.Write("namespace ")
    f.Write(namespace)
    f.WriteLine("")
    f.WriteLine("{")
    f.IncreaseIndent()

#------------------------------------------------------------------------------
##
#
def EndNamespaceOverride(f, document, namespace):
    f.DecreaseIndent()
    f.Write("} // namespace ")
    f.Write(namespace)
    f.Write("\n")
    f.WriteLine("//------------------------------------------------------------------------------")