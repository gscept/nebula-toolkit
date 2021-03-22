//------------------------------------------------------------------------------
//  entityloader.cc
//  @copyright (C) 2021 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "foundation/stdneb.h"
#include "entityloader.h"
#include "io/jsonreader.h"
#include "io/jsonwriter.h"
#include "io/ioserver.h"
#include "editor.h"
#include "cmds.h"
#include "commandmanager.h"
#include "game/propertyserialization.h"

namespace Editor
{

//------------------------------------------------------------------------------
/**
    Entity format is defined as:
    \code{.json}
    {
        "entities": {
            "[GUID]": {
                "name": String,
                "template": "TemplateName", // this should be changed to a GUID
				"properties": { // only lists the overridden properties
                    "PropertyName1": VALUE,
                    "PropertyName2": {
                        "FieldName1": VALUE,
                        "FieldName2": VALUE
                    }
                },
                "inactive_properties": { // only lists the overridden properties
                    ...
                },
                "removed_properties": { // lists properties that have been removed from the entity, that are part of the blueprint.
                    "PropertyName3",
                    "PropertyName4"
                }
            },
            "[GUID]": {
                ...
            }
        }
    }
    \endcode
*/
bool
LoadEntities(const char* filePath)
{
    IO::URI const file = filePath;
    Ptr<IO::JsonReader> reader = IO::JsonReader::Create();
	reader->SetStream(IO::IoServer::Instance()->CreateStream(file));
	if (reader->Open())
    {
		Util::Blob scratchBuffer = Util::Blob(128);
		Edit::CommandManager::BeginMacro();
		reader->SetToFirstChild();
		do
		{
			Editor::Entity editorEntity;
			Util::String entityName = reader->GetOptString("name", "unnamed_entity");
			bool const fromTemplate = reader->HasNode("template");
			if (fromTemplate)
			{
				Util::String templateName = reader->GetString("template");
				editorEntity = Edit::CreateEntity(templateName);
			}
			else
			{
				n_warning("Entities that are not instantiated from templates not yet supported!");
				return false;
			}

			Util::String const guid = reader->GetCurrentNodeName();
			Editor::state.editables[editorEntity.index].guid = Util::Guid::FromString(guid);

			if (reader->SetToFirstChild("properties"))
			{
				reader->SetToFirstChild();
				do
				{
					Util::StringAtom const propertyName = reader->GetCurrentNodeName();
					MemDb::PropertyId descriptor = MemDb::TypeRegistry::GetPropertyId(propertyName);
					if (descriptor == MemDb::PropertyId::Invalid())
					{
						n_warning("Warning: Entity '%s' contains invalid property named '%s'.\n", entityName.AsCharPtr(), propertyName.Value());
						continue;
					}

					
					if (!Game::HasProperty(Editor::state.editorWorld, editorEntity, descriptor))
					{
						n_warning("Warning: Entity '%s' contains property named '%s' that does not exist in template.\n", entityName.AsCharPtr(), propertyName.Value());
						continue;
					}

					if (scratchBuffer.Size() < MemDb::TypeRegistry::TypeSize(descriptor))
						scratchBuffer.Reserve(MemDb::TypeRegistry::TypeSize(descriptor));

					Game::PropertySerialization::Deserialize(reader, descriptor, scratchBuffer.GetPtr());
					Edit::SetProperty(editorEntity, descriptor, scratchBuffer.GetPtr());
				} while (reader->SetToNextChild());
				
				reader->SetToParent();
			}
			if (reader->SetToFirstChild("removed_properties"))
			{
				n_assert(reader->IsArray());
				
				Util::Array<Util::String> values;
				reader->Get<Util::Array<Util::String>>(values);
				for (auto s : values)
				{
					Game::PropertyId const pid = MemDb::TypeRegistry::GetPropertyId(s);
					if (pid != Game::PropertyId::Invalid())
						Edit::RemoveProperty(editorEntity, pid);
				}
				
				reader->SetToParent();
			}
		} while (reader->SetToNextChild());
		Edit::CommandManager::EndMacro();
        reader->Close();
        return true;
    }
    return false;
} 

//------------------------------------------------------------------------------
/**
*/
bool
SaveEntities(const char* uri)
{
    return false;
}
} // namespace Editor
