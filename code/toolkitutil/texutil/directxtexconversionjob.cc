//------------------------------------------------------------------------------
// win32textureconversionjob.cc
// (C) 2009 Radon Labs GmbH
//  (C) 2013-2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "directxtexconversionjob.h"
#include "io/uri.h"
#include "io/assignregistry.h"
#include "io/ioserver.h"
#include "system/systeminfo.h"

namespace ToolkitUtil
{
using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
DirectXTexConversionJob::DirectXTexConversionJob()
{
    System::SystemInfo systemInfo;
    this->toolPath.Format("toolkit:bin/%s/texconv", System::SystemInfo::PlatformAsString(systemInfo.GetPlatform()).AsCharPtr());
    this->SetDstFileExtension("dds");
}

static const char* 
GetTexConvFormat(TextureAttrs const& attrs)
{
    switch (attrs.GetPixelFormat())
    {
    case TextureAttrs::DXT1C:
    case TextureAttrs::DXT1A:  return "DXT1";
    case TextureAttrs::DXT3:   return "DXT3";
    case TextureAttrs::DXT5:   return "DXT5";
    case TextureAttrs::DXT5NM: return "BC5_UNORM";
    case TextureAttrs::U1555:  return "B5G5R5A1_UNORM";
    case TextureAttrs::U4444: return "B4G4R4A4_UNORM";
    case TextureAttrs::U565: return "B5G6R5_UNORM";
    case TextureAttrs::U8888: return "R8G8B8A8_UNORM_SRGB";
    case TextureAttrs::U888: return "BC7_UNORM";
    case TextureAttrs::BC4: return "BC4_UNORM";
    case TextureAttrs::BC5: "BC5_UNORM";
    case TextureAttrs::BC6H: return "BC6H_UNORM";
    case TextureAttrs::BC7: return "BC7_UNORM";
    case TextureAttrs::R8: return "R8_UINT";
    case TextureAttrs::R16: return "R16_UINT";
    case TextureAttrs::R16G16: return "R16G16_UINT";
    case TextureAttrs::DXGI: return attrs.GetDxgi().AsCharPtr();
    }
    return "BC7_UNORM";
}

//------------------------------------------------------------------------------
/**
    Start the conversion process. Returns false, if the job finished immediately.	
*/
bool
DirectXTexConversionJob::Convert()
{
    n_assert(this->toolPath.IsValid());
    n_assert(0 != this->logger);
    if (TextureConversionJob::Convert())
    {  
        URI srcPathUri(this->srcPath);
        URI dstPathUri(this->dstPath);
        URI tmpDirUri(this->tmpDir);
        
        // build command line args for TexConv
        String args = " -y -sepalpha -dx10 -nologo -timing ";
        bool isNormalMap = false;
        const TextureAttrs& attrs = this->textureAttrs;
        if ((attrs.GetPixelFormat() == TextureAttrs::DXT5NM) ||
            (attrs.GetPixelFormat() == TextureAttrs::BC5) ||
			(String::MatchPattern(this->srcPath, "*norm.*")) ||
			(String::MatchPattern(this->srcPath, "*normal.*")) ||
			(String::MatchPattern(this->srcPath, "*bump.*")))
        {
            isNormalMap = true;
        }
/*
        args.Append(" -w ");
        args.AppendInt(attrs.GetMaxWidth());
        args.Append(" -h ");
        args.AppendInt(attrs.GetMaxHeight());*/
        
        if (attrs.GetQuality() == TextureAttrs::High)
        {
            args.Append(" -bc x ");
        }
        else
        {
            args.Append(" -bc q ");
        }

        if (!attrs.GetGenMipMaps())
        {
            args.Append(" -m 1 ");
        }

        if (attrs.GetColorSpace() == TextureAttrs::sRGB)
        {
            args.Append(" -srgb ");
        }
        else
        {
            args.Append(" -srgbo ");
        }

        args.Append(" -f ");
        if (isNormalMap)
        {
            args.Append("BC5_UNORM");
        }
        else
        {
            args.Append(GetTexConvFormat(attrs));
        }
        
        
        args.Append(" \"");
        Util::String srcPath = srcPathUri.LocalPath();
        srcPath.ReplaceChars("/", '\\');
        args.Append(srcPath);
        args.Append("\" -o \"");
        Util::String tmpPath = tmpDirUri.LocalPath();
        tmpPath.ReplaceChars("/", '\\');
        args.Append(tmpPath);
        args.Append("\"");

        // launch nvdxt to perform the conversion        
        this->appLauncher.SetExecutable(this->toolPath);
        this->appLauncher.SetWorkingDirectory(this->srcPath.ExtractDirName());
        this->appLauncher.SetArguments(args);
#if __WIN32__
        //this->appLauncher.SetNoConsoleWindow(this->quiet);
#endif        
        if (!this->appLauncher.LaunchWait())
        {
            this->logger->Warning("Failed to launch converter tool '%s'!\n", this->toolPath.AsCharPtr());
            return false;
        }

        // copy converted texture from temp to export dir
        if (!this->CopyResult())
        {
            return false;
        }
    }
    return true;
}

}// namespace ToolkitUtil
