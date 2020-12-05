//------------------------------------------------------------------------------
//  texgenapp.cc
//  (C) 2020 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "texgenapp.h"
#include "toolkit-common/platform.h"
#include "io/ioserver.h"
#include "io/textreader.h"
#include "io/console.h"
#include "io/uri.h"
#include "timing/time.h"
#include <IL/il.h>
#include <IL/ilu.h>

namespace Toolkit
{
using namespace ToolkitUtil;
using namespace Util;
using namespace IO;

TexGenApp::TexGenApp() :
    ToolkitApp(),
    outputWidth(0),
    outputHeight(0)
{
    this->BGRA[0] = -1.0f;
    this->BGRA[1] = -1.0f;
    this->BGRA[2] = -1.0f;
    this->BGRA[3] = -1.0f;
}

//------------------------------------------------------------------------------
/**
*/
void
TexGenApp::Run()
{
    IoServer* ioServer = IoServer::Instance();

    ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

    Util::String inputPath = this->inputFile;

    ILuint outputImage;
    ilGenImages(1, &outputImage);
    
    ILint imgWidth = this->outputWidth;
    ILint imgHeight = this->outputHeight;
    
    ILint numChannels = 4;
    ILenum format = 0;
    ILenum type = 0;

    bool initialized = false;
    auto InitializeImage = [&initialized, &imgWidth, &imgHeight, &numChannels, &format, &type](ILuint outputImage, ILuint inputImage, float inputWidth, float inputHeight)
    {
        imgWidth = imgWidth > 0 ? imgWidth : inputWidth;
        imgHeight = imgHeight > 0 ? imgHeight : inputHeight;

        numChannels = ilGetInteger(IL_IMAGE_CHANNELS); //IL_IMAGE_BYTES_PER_PIXEL
        format = ilGetInteger(IL_IMAGE_FORMAT);
        type = ilGetInteger(IL_IMAGE_TYPE);

        ilBindImage(outputImage);
        ilTexImage(imgWidth, imgHeight, 1, numChannels, format, type, NULL);
        initialized = true;
        ilBindImage(NULL);
    };

    // first, load possible input image
    if (!this->inputFile.IsEmpty())
    {
        ILuint inputImage;
        ilGenImages(1, &inputImage);
        ilBindImage(inputImage);
        
        ILboolean res = ilLoadImage((ILstring)this->inputFile.AsCharPtr());
        if (IL_TRUE != res)
        {
            this->logger.Print("Could not load input file!\n");
            this->SetReturnCode(1);
            ilDeleteImages(1, &inputImage);
            ilDeleteImages(1, &outputImage);
            return;
        }

        ILint inputWidth = ilGetInteger(IL_IMAGE_WIDTH);
        ILint inputHeight = ilGetInteger(IL_IMAGE_HEIGHT);

        InitializeImage(outputImage, inputImage, inputWidth, inputHeight);
        ilBindImage(outputImage);
        iluImageParameter(ILU_FILTER, ILU_SCALE_BOX);
        ilCopyImage(inputImage);
        iluScale(imgWidth, imgHeight, 1);
        ilDeleteImages(1, &inputImage);
        ilBindImage(NULL);
    }

    // for each channel-specific image or color, override channel
    for (int channel = 0; channel < numChannels; channel++)
    {
        if (!this->outputBGRA[channel].IsEmpty())
        {
            Util::Array<Util::String> tokens = this->outputBGRA[channel].Tokenize(";");
            if (tokens.Size() < 2)
            {
                this->logger.Print("invalid input specified for channel specific images!\n");
                this->SetReturnCode(1);
                return;
            }

            Util::String& filePath = tokens[0];
            Util::String& channelCharacter = tokens[1];
            int inputChannelIndex = 0;
            if (channelCharacter == "b")
                inputChannelIndex = 0;
            else if (channelCharacter == "g")
                inputChannelIndex = 1;
            else if (channelCharacter == "r")
                inputChannelIndex = 2;
            else if (channelCharacter == "a")
                inputChannelIndex = 3;
            else
            {
                this->logger.Print("invalid channel specified for channel-specific images!\n");
                this->SetReturnCode(1);
                return;
            }

            ILuint channelImage;
            ilGenImages(1, &channelImage);
            ilBindImage(channelImage);

            ILboolean res = ilLoadImage((ILstring)filePath.AsCharPtr());
            if (IL_TRUE != res)
            {
                this->logger.Print("Could not load channel input file!\n");
                this->SetReturnCode(1);
                ilDeleteImages(1, &channelImage);
                ilDeleteImages(1, &outputImage);
                return;
            }

            ILint channelWidth = ilGetInteger(IL_IMAGE_WIDTH);
            ILint channelHeight = ilGetInteger(IL_IMAGE_HEIGHT);
            ILint numInputChannels = ilGetInteger(IL_IMAGE_CHANNELS);

            if (!initialized)
            {
                InitializeImage(outputImage, channelImage, channelWidth, channelHeight);
            }

            ilBindImage(channelImage);
            ILubyte* in = ilGetData();
            ilBindImage(outputImage);
            ILubyte* out = ilGetData();

            float const dW = (float)channelWidth / (float)imgWidth;
            float const dH = (float)channelHeight / (float)imgHeight;

            for (int y = 0; y < imgHeight; y++)
            {
                for (int x = 0; x < imgWidth; x++)
                {
                    int const oPixel = (x + y * imgWidth) * numChannels;
                    int const iPixel = ((int)(x * dW) + (int)(y * dH) * channelWidth) * numInputChannels;
                    out[oPixel + channel] = in[iPixel + inputChannelIndex];
                }
            }
            ilBindImage(NULL);
            ilDeleteImages(1, &channelImage);
        }
    }

    for (int channel = 0; channel < numChannels; channel++)
    {
        if (this->outputBGRA[channel].IsEmpty())
        {
            if (this->BGRA[channel] != -1.0f)
            {
                // override with color
                ilBindImage(outputImage);
                ILubyte* out = ilGetData();

                for (int y = 0; y < imgHeight; y++)
                {
                    for (int x = 0; x < imgWidth; x++)
                    {
                        int pixel = (x + y * imgWidth) * numChannels;
                        uint val = (uint)(this->BGRA[channel] * 255.f);
                        out[pixel + channel] = (ILubyte)val;
                    }
                }
            }
            else if (this->inputFile.IsEmpty())
            {
                // override with zeroes
                ilBindImage(outputImage);
                ILubyte* out = ilGetData();

                for (int y = 0; y < imgHeight; y++)
                {
                    for (int x = 0; x < imgWidth; x++)
                    {
                        int pixel = (x + y * imgWidth) * numChannels;
                        out[pixel + channel] = 0;
                    }
                }
            }
        }
    }
    
    ilBindImage(outputImage);
    ilEnable(IL_FILE_OVERWRITE);
    if (ilSaveImage(this->outputFile.AsCharPtr()))
    {
        this->logger.Print("Image generated to %s\n", this->outputFile.AsCharPtr());
    }
    else
    {
        this->logger.Print("Failed to generate image to %s\n", this->outputFile.AsCharPtr());
        this->SetReturnCode(1);
        return;
    }

    ilDeleteImages(1, &outputImage);
}

//------------------------------------------------------------------------------
/**
*/
bool TexGenApp::ParseCmdLineArgs()
{
    if (ToolkitApp::ParseCmdLineArgs())
    {
        if (!this->args.HasArg("-o"))
        {
            this->logger.Print("No output file specified!\n");
            this->SetReturnCode(1);
            return false;
        }
        this->outputFile = this->args.GetString("-o");
        
        if (this->args.HasArg("-i"))
        {
            this->inputFile = this->args.GetString("-i");
        }

        constexpr char* bgra = "bgra";
        for (int i = 0; i < 4; i++)
        {
            Util::String arg;
            arg.Format("-%c", bgra[i]);
            if (this->args.HasArg(arg))
            {
                Util::String val = this->args.GetString(arg);
                if (val.IsValidFloat() || val.IsValidInt())
                {
                    this->BGRA[i] = val.AsFloat();
                }
                else
                {
                    this->outputBGRA[i] = val;
                }
            }
        }

        for (int i = 0; i < 4; i++)
        {
            Util::String arg;
            arg.Format("-%c", bgra[i]);
            
        }

        this->outputWidth = this->args.GetInt("-w", 0);
        this->outputHeight = this->args.GetInt("-h", 0);

        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
/**
*/
void
TexGenApp::ShowHelp()
{
    n_printf("Nebula texture generator %s\n"
            "[Toolkit %s]\n"
            "(C) 2020 Individual Authors, see AUTHORS file.\n\n", this->GetAppVersion().AsCharPtr(), this->GetToolkitVersion().AsCharPtr());
    n_printf("Available options:"
             "-help                        -- Print this help message\n"
             "-o <file>                    -- Output file path and extension\n"
             "-w [width]                   -- Output file's width. Will infer from input if not specified.\n"
             "-h [height]                  -- Output file's height. Will infer from input if not specified.\n"
             "-i <file>                    -- Input file path and extension\n"
             "-(r|g|b|a) [0, 1]            -- Set a channel to a value between 0 and 1. Takes priority over '-i'\n"
             "-(r|g|b|a) <file>;(r|g|b|a)  -- Set output's red, green, blue or alpha channel from a specific channel in an input file.\n"
             "                                Can be specified multiple times.\n"
             "                                Takes priority over '-(r/g/b/a)' and '-i'\n\n"
             "Example: texgen -o foo.bmp -i bar.bmp -r gnyrf.bmp;b -a 0.5\n"
             "     - This will create output file foo.bmp, using bar.bmp as base, overriding the red channel with the blue channel from gnyrf.bmp and settings the alpha channel to 0.5 across the entire image.\n\n"
    );
}


} // namespace ToolkitUtil
