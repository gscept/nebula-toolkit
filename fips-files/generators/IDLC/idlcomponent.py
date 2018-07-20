import genutil as util
import idltypes as IDLTypes

#------------------------------------------------------------------------------
## Constants
#
PACKED_PER_ATTRIBUTE = 0
PACKED_PER_INSTANCE = 1


#------------------------------------------------------------------------------
##
#
def WriteIncludes(f, attributeLibraries):
    f.WriteLine('#include "game/component/componentdata.h"')
    f.WriteLine('#include "game/component/basecomponent.h"')
    f.WriteLine('#include "game/attr/attributedefinitionbase.h"')

    for lib in attributeLibraries:
        f.WriteLine('#include "{}"'.format(lib))

#------------------------------------------------------------------------------
##
#
def WriteClassDeclaration(f, document, component, componentName):
    className = '{}Base'.format(componentName)
    dataLayout = PACKED_PER_ATTRIBUTE
    hasAttributes = "attributes" in component

    attributeNotFoundError = 'No attribute named {} could be found by component compiler! Please make sure it\'s defined or imported as a dependency in the current .nidl file.'

    if hasAttributes and not "attributes" in document:
        util.error('Component has attributes attached but none could be found by the component compiler! Please make sure they\'re defined or imported as a dependency in the current .nidl file.')
        
    # Read dataLayout from NIDL file
    if "dataLayout" in component:
        dataLayout = GetDataLayout(component["dataLayout"])

    # Write instance structure for PPI components
    if dataLayout == PACKED_PER_INSTANCE:
        f.WriteLine('struct {}Instance'.format(componentName))
        f.WriteLine("{")
        f.IncreaseIndent()
        if hasAttributes:
            for attributeName in component["attributes"]:
                if not attributeName in document["attributes"]:
                    util.error(attributeNotFoundError.format(attributeName))
                f.WriteLine('{} {};'.format(IDLTypes.GetTypeString(document["attributes"][attributeName]["type"]), attributeName))
        f.DecreaseIndent()
        f.WriteLine("}")
        f.WriteLine("")
        f.WriteLine("//------------------------------------------------------------------------------")    
    
    # Declare class
    f.WriteLine('class {} : public Game::BaseComponent'.format(className))
    f.WriteLine("{")

    f.IncreaseIndent()
    f.WriteLine('__DeclareClass({})'.format(className))
    f.DecreaseIndent()
    f.WriteLine("public:")
    f.IncreaseIndent()
    f.WriteLine("/// Default constructor")
    f.WriteLine('{}();'.format(className))
    f.WriteLine("/// Default destructor")
    f.WriteLine('~{}();'.format(className))
    f.WriteLine("")
    f.WriteLine("/// Registers an entity to this component. Entity is inactive to begin with.")
    f.WriteLine("void RegisterEntity(const Game::Entity& entity);")
    f.WriteLine("")
    f.WriteLine("/// Deregister Entity. This checks both active and inactive component instances.")
    f.WriteLine("void DeregisterEntity(const Game::Entity& entity);")
    f.WriteLine("")
    f.WriteLine("/// Deregister all entities from both inactive and active. Garbage collection will take care of freeing up data.")
    f.WriteLine("void DeregisterAll();")
    f.WriteLine("")
    f.WriteLine("/// Deregister all non-alive entities from both inactive and active. This can be extremely slow!")
    f.WriteLine("void DeregisterAllDead();")
    f.WriteLine("")
    f.WriteLine("/// Cleans up right away and frees any memory that does not belong to an entity. This can be extremely slow!")
    f.WriteLine("void CleanData();")
    f.WriteLine("")
    f.WriteLine("/// Destroys all instances of this component, and deregisters every entity.")
    f.WriteLine("void DestroyAll();")
    f.WriteLine("")
    f.WriteLine("/// Checks whether the entity is registered. Checks both inactive and active datasets.")
    f.WriteLine("bool IsRegistered(const Game::Entity& entity) const;")
    f.WriteLine("")
    f.WriteLine("/// Activate entity component instance.")
    f.WriteLine("void Activate(const Game::Entity& entity);")
    f.WriteLine("")
    f.WriteLine("/// Deactivate entity component instance. Instance state will remain after reactivation but not after deregistering.")
    f.WriteLine("void Deactivate(const Game::Entity& entity);")
    f.WriteLine("")
    f.WriteLine("/// Returns the index of the data array to the component instance")
    f.WriteLine("/// Note that this only checks the active dataset")
    f.WriteLine("uint32_t GetInstance(const Game::Entity& entity) const;")
    f.WriteLine("")
    f.WriteLine("/// Returns the owner entity id of provided instance id")
    f.WriteLine("Game::Entity GetOwner(const uint32_t& instance) const;")
    f.WriteLine("")
    f.WriteLine("/// Optimize data array and pack data")
    f.WriteLine("SizeT Optimize();")
    f.WriteLine("")
    f.WriteLine("/// Returns an attribute value as a variant from index.")
    f.WriteLine("Util::Variant GetAttributeValue(uint32_t instance, IndexT attributeIndex) const;")
    f.WriteLine("/// Returns an attribute value as a variant from attribute id.")
    f.WriteLine("Util::Variant GetAttributeValue(uint32_t instance, Attr::AttrId attributeId) const;")
    f.WriteLine("")
    f.DecreaseIndent()
    f.WriteLine("protected:")
    f.IncreaseIndent()
    f.WriteLine("/// Read/write access to attributes.")
    if hasAttributes:
        for attributeName in component["attributes"]:
            if not attributeName in document["attributes"]:
                util.error(attributeNotFoundError.format(attributeName))
            f.WriteLine('const {}& GetAttr{}(const uint32_t& instance);'.format(IDLTypes.GetTypeString(document["attributes"][attributeName]["type"]), attributeName.capitalize()))
            f.WriteLine('void SetAttr{}(const uint32_t& instance, const {}& value);'.format(attributeName.capitalize(), IDLTypes.GetTypeString(document["attributes"][attributeName]["type"])))
    f.WriteLine("")  
    f.DecreaseIndent()
    f.WriteLine("private:")
    f.IncreaseIndent()

    templateArgs = ""

    if dataLayout == PACKED_PER_ATTRIBUTE:
        numAttributes = len(component["attributes"])
        for i, attributeName in enumerate(component["attributes"]):
            if not attributeName in document["attributes"]:
                util.error(attributeNotFoundError.format(attributeName))
            templateArgs += IDLTypes.GetTypeString(document["attributes"][attributeName]["type"])
            if i != (numAttributes - 1):
                templateArgs += ", "
    else:
        templateArgs += '{}Instance'.format(componentName)

    componentData = 'Game::ComponentData<{}> {};'

    f.WriteLine("/// Holds all active entities data")
    f.WriteLine(componentData.format(templateArgs, "data"))
    f.WriteLine("/// Holds all inactive component instances.")
    f.WriteLine(componentData.format(templateArgs, "inactiveData"))
    f.DecreaseIndent()

    f.WriteLine("};")
    f.WriteLine("")

#------------------------------------------------------------------------------
##
#
def WriteClassImplementation(f, document, component, componentName):
    return

#------------------------------------------------------------------------------
##
#
def GetDataLayout(string):
    if string == "PACKED_PER_ATTRIBUTE":
        return PACKED_PER_ATTRIBUTE
    elif string == "PACKED_PER_INSTANCE":
        return PACKED_PER_INSTANCE