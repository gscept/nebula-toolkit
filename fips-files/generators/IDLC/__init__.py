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
        self.document = sjson.loads(fstream.read());


    #------------------------------------------------------------------------------
    ##
    #
    def GenerateHeader(self, hdrPath) :
        f = filewriter.FileWriter()

        # TODO: Parse all dependencies

        attributeLibraries = []

        # TODO: Add attribute libraries so that we can send them on to the components that need them

        # Generate attributes include file
        if "attributes" in self.document:
            fileName = '{}attributes.h'.format(self.documentFileName).lower()
            fullFilePath = '{}attributes.h'.format(self.documentBaseName).lower()
            

            # TODO: We need to find the correct path to include in our components for this file.
            attributeLibraries.append(fileName)

            f.Open(fullFilePath)
            IDLDocument.WriteIncludeHeader(f)
            IDLDocument.WriteAttributeLibraryDeclaration(f)
            IDLDocument.BeginNamespaceOverride(f, self.document, "Attr")
            IDLAttribute.WriteAttributeHeaderDeclarations(f, self.document)
            IDLDocument.EndNamespaceOverride(f, self.document, "Attr")
            f.Close()

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
                IDLDocument.WriteSourceHeader(f, self.documentFileName)
                IDLDocument.AddInclude(f, '{}base.h'.format(componentName).lower())
                f.WriteLine("")
                componentWriter = IDLComponent.ComponentClassWriter(f, self.document, component, componentName)
                IDLDocument.BeginNamespace(f, self.document)
                componentWriter.WriteClassImplementation()
                IDLDocument.EndNamespace(f, self.document)
                f.Close()
        
