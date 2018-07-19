import os, platform
import idldocument as IDLDocument
import idlattribute as IDLAttribute
import sjson
import filewriter

class IDLCodeGenerator:
    def __init__(self):
        self.document = None
        self.documentPath = ""

    #------------------------------------------------------------------------------
    ##
    #
    def SetDocument(self, input) :
        self.documentPath = input
        fstream = open(self.documentPath, 'r')
        self.document = sjson.loads(fstream.read());


    #------------------------------------------------------------------------------
    ##
    #
    def GenerateHeader(self, hdrPath) :
        f = filewriter.FileWriter()
        f.Open(hdrPath)
        IDLDocument.WriteIncludeHeader(f)
        IDLDocument.WriteAttributeLibraryDeclaration(f)
        IDLDocument.BeginNamespace(f, self.document)
        IDLAttribute.WriteAttributeHeaderDeclarations(f, self.document)
        IDLDocument.EndNamespace(f, self.document)
        f.Close()

    #------------------------------------------------------------------------------
    ##
    #
    def GenerateSource(self, srcPath) :
        f = filewriter.FileWriter()
        f.Open(srcPath)
        f.Write("Source file test")
        f.Close()
