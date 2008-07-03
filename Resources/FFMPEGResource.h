#ifndef __FFMPEG_RESOURCE__
#define __FFMPEG_RESOURCE__

#include <Core/IModule.h>
#include <Resources/IMovieResource.h>
#include <Resources/ResourcePlugin.h>
#include <Meta/SDL.h>

#ifdef _WIN32
    #include <avcodec.h>
    #include <avformat.h>
    #include <swscale.h>
#else
#ifdef __APPLE__
extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}
#else
    #include <ffmpeg/avcodec.h>
    #include <ffmpeg/avformat.h>
    #include <ffmpeg/swscale.h>
    #include "thetypes.h"
#endif
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
class FFMPEGPlugin : public ResourcePlugin<IMovieResource> {
public:
    FFMPEGPlugin();
    IMovieResourcePtr CreateResource(string file);
};

class FFMPEGResource : public IMovieResource {
private:
    SwsContext* img_convert_ctx;
    AVFormatContext* pFormatCtx;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;
    unsigned char* data;

    int id, videoStream;
    string filename;
    bool loop, pause, loaded;
    unsigned int numberOfChannels, bytesPerColor, lineSize;
    unsigned int height, width, movieHeight, movieWidth, frameNumber;
    float movie_spf, timeForTwoFrames;
    double time;

    void BindTexture();
    void DecodeOneFrame();

public:
    FFMPEGResource(string filename, bool loop = true);
    ~FFMPEGResource();

    // from IMovieResource
    void Pause(bool pause);
    bool Ended();
    void Restart();
    int GetMovieHeight();
    int GetMovieWidth();

    // from IModule
    void Initialize();
    void Deinitialize();
    void Process(const float dt, const float percent);
    bool IsTypeOf(const std::type_info& inf);

    // from IResource
    virtual void Load();
    virtual void Unload();
    //virtual void ReverseVertecally();

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
