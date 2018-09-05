import os, platform
import idldocument as IDLDocument
import idlattribute as IDLAttribute
import idlcomponent as IDLComponent
import sjson
import filewriter
import genutil as util
import ntpath

class IDLCodeGenerator:
    def __init__(self):
        self.document = None
        self.documentPath = ""

    #------------------------------------------------------------------------------
    ##
    #
    def SetDocument(self, input) :
        self.documentPath = input
        self.documentBaseName = os.path.splitext(input)[0]
        self.documentDirName = os.path.dirname(self.documentBaseName)

        head, tail = ntpath.split(self.documentBaseName)
        self.documentFileName = tail or ntpath.basename(head)

        fstream = open(self.documentPath, 'r')
        self.document = sjson.loads(fstream.read())


    #------------------------------------------------------------------------------
    ##
    #
    def GenerateHeader(self, hdrPath) :
        f = filewriter.FileWriter()

        attributeLibraries = []

        # Generate attributes include file
        if "attributes" in self.document:
            fileName = '{}.h'.format(self.documentFileName).lower()
            headerFilePath = '{}.h'.format(self.documentBaseName).lower()
            sourceFilePath = '{}.cc'.format(self.documentBaseName).lower()
            
            # TODO: We need to find the correct path to include in our components for this file.
            attributeLibraries.append(fileName)

            #---------------------
            # Create header file
            f.Open(headerFilePath)
            IDLDocument.WriteIncludeHeader(f)
            IDLDocument.WriteAttributeLibraryDeclaration(f)
            
            IDLDocument.BeginNamespace(f, self.document)
            IDLAttribute.WriteEnumeratedTypes(f, self.document)
            IDLDocument.EndNamespace(f, self.document)
            f.WriteLine("")

            IDLDocument.BeginNamespaceOverride(f, self.document, "Attr")
            IDLAttribute.WriteAttributeHeaderDeclarations(f, self.document)
            IDLDocument.EndNamespaceOverride(f, self.document, "Attr")
            f.Close()

            #---------------------
            # Create source file
            f.Open(sourceFilePath)
            IDLDocument.WriteSourceHeader(f, self.documentFileName)
            IDLDocument.AddInclude(f, fileName)

            IDLDocument.BeginNamespaceOverride(f, self.document, "Attr")
            IDLAttribute.WriteAttributeDefinitions(f, self.document)
            IDLDocument.EndNamespaceOverride(f, self.document, "Attr")
            f.Close()

        # Add additional dependencies to document.
        if "dependencies" in self.document:
            for dependency in self.document["dependencies"]:
                fileName = '{}.h'.format(os.path.splitext(dependency)[0]).lower()
                attributeLibraries.append(fileName)

                fstream = open(dependency, 'r')
                depDocument = sjson.loads(fstream.read())

                deps = depDocument["attributes"]
                # Add all attributes to this document
                self.document["attributes"].update(deps)


        # Generate components base classes headers
        if "components" in self.document:
            for componentName, component in self.document["components"].iteritems():
                f.Open('{}/{}base.h'.format(self.documentDirName, componentName).lower())
                componentWriter = IDLComponent.ComponentClassWriter(f, self.document, component, componentName)
                IDLDocument.WriteIncludeHeader(f)
                IDLDocument.WriteIncludes(f, self.document)
                IDLComponent.WriteIncludes(f, attributeLibraries)
                IDLDocument.BeginNamespace(f, self.document)
                componentWriter.WriteClassDeclaration()
                IDLDocument.EndNamespace(f, self.document)
                f.Close()




    #------------------------------------------------------------------------------
    ##
    #
    def GenerateSource(self, srcPath) :
        f = filewriter.FileWriter()

        if "components" in self.document:
            for componentName, component in self.document["components"].iteritems():
                f.Open('{}/{}base.cc'.format(self.documentDirName, componentName).lower())
                IDLDocument.WriteSourceHeader(f, "{}base".format(self.documentFileName))
                IDLDocument.AddInclude(f, '{}base.h'.format(componentName).lower())
                f.WriteLine("")
                componentWriter = IDLComponent.ComponentClassWriter(f, self.document, component, componentName)
                IDLDocument.BeginNamespace(f, self.document)
                componentWriter.WriteClassImplementation()
                IDLDocument.EndNamespace(f, self.document)
                f.Close()
        
