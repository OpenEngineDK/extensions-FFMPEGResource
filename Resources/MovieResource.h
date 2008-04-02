#ifndef __MOVIE_RESOURCE__
#define __MOVIE_RESOURCE__

#include <Core/IModule.h>
#include <Resources/ITextureResource.h>
#include <Meta/SDL.h>

#if defined (_WIN32) or (__APPLE__)
extern "C" {
    #include <avcodec.h>
    #include <avformat.h>
    #include <swscale.h>
}
#else
    #include <ffmpeg/avcodec.h>
    #include <ffmpeg/avformat.h>
    #include <ffmpeg/swscale.h>
#include "thetypes.h"
#endif

#include <Meta/OpenGL.h>
#include <iostream>
#include <stdio.h>

namespace OpenEngine {
namespace Resources {

using OpenEngine::Core::IModule;
using OpenEngine::Resources::ITextureResource;
using namespace std;

/**
 * Movie texture resource plug-in.
 *
 * @class MoviePlugin MovieResource.h Resources/MovieResource.h
 */
class MoviePlugin : public ITextureResourcePlugin {
public:
	MoviePlugin();
    ITextureResourcePtr CreateResource(string file);
};

class MovieResource : public IModule, public ITextureResource {
private:
    //--#ifdef _WIN32
    SwsContext *img_convert_ctx;
    //--#endif

    AVFormatContext *pFormatCtx;
    int             videoStream;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;

    int id;
    string filename;
    unsigned char* data;
    bool loop, pause;
    unsigned int numberOfChannels, bytesPerColor, height, width, movieHeight, movieWidth, frameNumber, lineSize;
    float movie_spf, timeForTwoFrames;
    double time;

public:
    MovieResource(string filename, bool loop = true);
    ~MovieResource();
    void Pause(bool pause);
    bool Ended();
    void Restart();
    void DecodeOneFrame();
	int GetMovieHeight();
	int GetMovieWidth();

    // from IModule
    void Initialize();
    void Deinitialize();
    void Process(const float dt, const float percent);
    bool IsTypeOf(const std::type_info& inf);

    // from IResource
	void Load();
    void Unload();

    // from TextureResource
    int GetID();
	void SetID(int id);
	int GetHeight();
	int GetWidth();
	int GetDepth();
	unsigned char* GetData();
};

} //NS Resources
} //NS OpenEngine

#endif //__MOVIE_RESOURCE__
