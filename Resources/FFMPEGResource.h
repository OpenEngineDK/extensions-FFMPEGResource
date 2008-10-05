#ifndef __FFMPEG_RESOURCE__
#define __FFMPEG_RESOURCE__

#include <Core/EngineEvents.h>
#include <Core/IModule.h>
#include <Resources/IMovieResource.h>
#include <Resources/ResourcePlugin.h>

#include <Meta/FFMPEG.h>

#include <boost/serialization/weak_ptr.hpp>

#include <iostream>
#include <stdio.h>

namespace OpenEngine {
namespace Resources {

using OpenEngine::Resources::ITextureResource;
using namespace OpenEngine::Core;
using namespace std;

class FFMPEGResource;

/**
 * Texture resource smart pointer.
 */
typedef boost::shared_ptr<FFMPEGResource> FFMPEGResourcePtr;

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

    void RebindTexture();
    void DecodeOneFrame();

    boost::weak_ptr<FFMPEGResource> weak_this;
    FFMPEGResource(string filename, bool loop = true);
    FFMPEGResource(const FFMPEGResource& res);
    FFMPEGResource& operator=(const FFMPEGResource& res);

public:
    static FFMPEGResourcePtr Create(string filename, bool loop = true);
    ~FFMPEGResource();

    // from IMovieResource
    void Pause(bool pause);
    bool Ended();
    void Restart();
    unsigned int GetMovieHeight();
    unsigned int GetMovieWidth();
    ColorFormat GetColorFormat();

    // from IModule
    virtual void Handle(InitializeEventArg);
    virtual void Handle(DeinitializeEventArg);
    virtual void Handle(ProcessEventArg);

    // from IResource
    virtual void Load();
    virtual void Unload();
    //virtual void ReverseVertecally();

    // from TextureResource
    int GetID();
    void SetID(int id);
    unsigned int GetHeight();
    unsigned int GetWidth();
    unsigned int GetDepth();
    unsigned char* GetData();
};

} //NS Resources
} //NS OpenEngine

#endif //__MOVIE_RESOURCE__
