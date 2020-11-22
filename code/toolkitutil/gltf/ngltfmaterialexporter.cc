//------------------------------------------------------------------------------
//  ngltfmaterialextractor.cc
//  (C) 2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ngltfmaterialexporter.h"
#include "io/ioserver.h"
#include "io/binarywriter.h"
#include "io/filestream.h"
#include "surface/surfacebuilder.h"

using namespace Util;
namespace ToolkitUtil
{
//------------------------------------------------------------------------------
/**
*/
NglTFMaterialExtractor::NglTFMaterialExtractor()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
NglTFMaterialExtractor::~NglTFMaterialExtractor()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
NglTFMaterialExtractor::ExportAll()
{
	String surfaceExportPath = "sur:" + this->catName + "/" + this->subDir;
	this->texCatDir = "tex:" + this->catName;
	this->textureDir = texCatDir + "/" + this->subDir + "/";

	if (this->doc->materials.Size() > 0)
	{
		if (IO::IoServer::Instance()->DirectoryExists(surfaceExportPath))
		{
			// delete all previously generated images
			if (!IO::IoServer::Instance()->DeleteDirectory(surfaceExportPath))
				n_warning("Warning: NglTFMaterialExtractor: Could not delete old directory for gltf-specific surfaces.\n");
		}

		// Generate surfaces
		for (IndexT i = 0; i < this->doc->materials.Size(); i++)
		{
			Gltf::Material const& material = this->doc->materials[i];
			
			SurfaceBuilder builder;
			builder.SetDstDir(surfaceExportPath);
			
			Util::String templateName;
			if (material.alphaMode == Gltf::Material::AlphaMode::Opaque)
			{
				if (!material.doubleSided)
					templateName = "GLTF Static";
				else
					templateName = "GLTF Static + DoubleSided";
			}
			else if (material.alphaMode == Gltf::Material::AlphaMode::Blend)
			{
				if (!material.doubleSided)
					templateName = "GLTF Static + AlphaBlend";
				else
					templateName = "GLTF Static + AlphaBlend + DoubleSided";
			}
			else
			{
				if (!material.doubleSided)
					templateName = "GLTF Static + AlphaMask";
				else
					templateName = "GLTF Static + AlphaMask + DoubleSided";

				builder.AddParam("alphaCutoff", Util::String::FromFloat(material.alphaCutoff));
			}

			builder.SetMaterial(templateName);
			this->ExtractMaterial(builder, material);
			builder.ExportBinary(surfaceExportPath + "/" + material.name + ".sur");
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
NglTFMaterialExtractor::ExtractMaterial(SurfaceBuilder& builder, Gltf::Material const& material)
{
	if (material.pbrMetallicRoughness.baseColorTexture.index != -1)
	{
		int baseColorTexture = this->doc->textures[material.pbrMetallicRoughness.baseColorTexture.index].source;
		if (this->doc->images[baseColorTexture].embedded)
			builder.AddParam("baseColorTexture", this->textureDir + Util::String::FromInt(baseColorTexture));
		else
		{
			// texture is not embedded, we need to find the correct path to it
			n_error("TODO");
		}
	}
	else
	{
		builder.AddParam("baseColorTexture", "tex:system/white");
	}

	if (material.pbrMetallicRoughness.metallicRoughnessTexture.index != -1)
	{
		int metallicRoughnessTexture = this->doc->textures[material.pbrMetallicRoughness.metallicRoughnessTexture.index].source;

		if (this->doc->images[metallicRoughnessTexture].embedded)
			builder.AddParam("metallicRoughnessTexture", this->textureDir + Util::String::FromInt(metallicRoughnessTexture));
		else
		{
			// texture is not embedded, we need to find the correct path to it
			n_error("TODO");
		}
	}
	else
	{
		builder.AddParam("metallicRoughnessTexture", "tex:system/white");
	}

	int normalTexture = this->doc->textures[material.normalTexture.index].source;
	if (normalTexture > -1)
	{
		builder.AddParam("normalTexture", this->textureDir + Util::String::FromInt(normalTexture));
	}
	else
	{
		builder.AddParam("normalTexture", "tex:system/normal_color");
	}

	if (material.emissiveTexture.index != -1)
	{
		int emissiveTexture = this->doc->textures[material.emissiveTexture.index].source;
		if (emissiveTexture > -1)
		{
			builder.AddParam("emissiveTexture", this->textureDir + Util::String::FromInt(emissiveTexture));
		}
		else
		{
			// texture is not embedded, we need to find the correct path to it
			n_error("TODO");
		}
	}
	else
	{
		builder.AddParam("emissiveTexture", "tex:system/black");
	}

	if (material.occlusionTexture.index != -1)
	{
		int occlusionTexture = this->doc->textures[material.occlusionTexture.index].source;
		if (occlusionTexture > -1)
		{
			builder.AddParam("occlusionTexture", this->textureDir + Util::String::FromInt(occlusionTexture));
		}
		else
		{
			// texture is not embedded, we need to find the correct path to it
			n_error("TODO");
		}
	}
	else
	{
		builder.AddParam("occlusionTexture", "tex:system/white");
	}

	builder.AddParam("baseColorFactor", Util::String::FromVec4(material.pbrMetallicRoughness.baseColorFactor));
	builder.AddParam("metallicFactor", Util::String::FromFloat(material.pbrMetallicRoughness.metallicFactor));
	builder.AddParam("roughnessFactor", Util::String::FromFloat(material.pbrMetallicRoughness.roughnessFactor));
	builder.AddParam("emissiveFactor", Util::String::FromVec4(material.emissiveFactor.vec));
}

} // namespace ToolkitUtil
//------------------------------------------------------------------------------