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
		reader->SetToFirstChild("entities");
		Util::Blob scratchBuffer = Util::Blob(128);
		Edit::CommandManager::BeginMacro("Load entities", false);
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
SaveEntities(const char* filePath)
{
    IO::URI const file = filePath;
    Ptr<IO::JsonWriter> writer = IO::JsonWriter::Create();
    writer->SetStream(IO::IoServer::Instance()->CreateStream(file));
    if (writer->Open())
    {
        writer->BeginObject("entities");

		Game::FilterCreateInfo filterInfo;
		filterInfo.inclusive[0] = Game::GetPropertyId("Owner"_atm);
		filterInfo.access[0] = Game::AccessMode::READ;
		filterInfo.numInclusive = 1;

		Game::Filter filter = Game::CreateFilter(filterInfo);
		Game::Dataset data = Game::Query(state.editorWorld, filter);
		Game::PropertyId ownerPid = Game::GetPropertyId("Owner"_atm);

		for (int v = 0; v < data.numViews; v++)
		{
			Game::Dataset::CategoryTableView const& view = data.views[v];
			Editor::Entity const* const entities = (Editor::Entity*)view.buffers[0];

			for (IndexT i = 0; i < view.numInstances; ++i)
			{
				Editor::Entity const& editorEntity = entities[i];
				Editable& edit = state.editables[editorEntity.index];
				Game::EntityMapping const mapping = Game::GetEntityMapping(Editor::state.editorWorld, editorEntity);
				writer->BeginObject(edit.guid.AsString().AsCharPtr());
				writer->Add(edit.name, "name");
				writer->Add("MovingEntity/cube", "template"); // FIXME
				MemDb::Table const& table = Game::GetWorldDatabase(Editor::state.editorWorld)->GetTable(view.cid);
				IndexT col = 0;
				writer->BeginObject("properties");
				for (auto pid : table.properties)
				{
					if (pid != ownerPid)
					{
						void* buffer = Game::GetInstanceBuffer(Editor::state.editorWorld, table.tid, pid);
						if (buffer != nullptr)
							Game::PropertySerialization::Serialize(writer, pid, ((byte*)buffer) + MemDb::TypeRegistry::TypeSize(pid) * mapping.instance);
						else
							n_printf("FIXME: Serialization of flag type properties not implemented yet!\n");
					}

					col++;
				}
				writer->End();
				writer->End();
			}
		}

		Game::DestroyFilter(filter);

		writer->End();
        return true;
    }

    return false;
}
} // namespace Editor
