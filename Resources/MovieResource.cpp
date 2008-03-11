#include "MovieResource.h"

#include <Logging/Logger.h>
#include <Core/Exceptions.h>
#include <Utils/Convert.h>
#include <Utils/Timer.h>
#include <math.h>

namespace OpenEngine {
namespace Resources {

using OpenEngine::Core::Exception;
using OpenEngine::Utils::Timer;
using OpenEngine::Utils::Convert;

MoviePlugin::MoviePlugin() {
    this->AddExtension("mov");
    this->AddExtension("mp4");
    this->AddExtension("avi");
}

ITextureResourcePtr MoviePlugin::CreateResource(string file) {
  return ITextureResourcePtr(new MovieResource(file, false));
}

MovieResource::MovieResource(string filename, bool loop) : id(0), filename(filename), loop(loop) {
    pause = false;
    videoStream = -1;
    numberOfChannels = 4;
    bytesPerColor = 3;
    frameNumber = 0;

    //--#ifdef _WIN32
    img_convert_ctx = NULL;
    //--#endif

    // Register all formats and codecs
    av_register_all();

    // Open video file
    if (av_open_input_file(&pFormatCtx, filename.c_str(), NULL, 0, NULL) != 0)
        throw Exception("Couldn't open file: " + filename);

    // Retrieve stream information
    if (av_find_stream_info(pFormatCtx) < 0)
        throw Exception("Couldn't find stream information");

    // Find the first video stream
    for (unsigned int i=0; i<pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
            videoStream=i;
            break;
        }
    if (videoStream == -1)
        throw Exception("Didn't find a video stream");
  
    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;

    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL)
        throw Exception("Unsupported codec: Codec not found");

    // Open codec
    if (avcodec_open(pCodecCtx, pCodec) < 0)
        throw Exception("Could not open codec");

    float movie_fps = av_q2d(pFormatCtx->streams[videoStream]->r_frame_rate);
    if (movie_fps == 0) //avoid div by zero
        throw Exception("movie_fps not found");
    movie_spf = ((1/movie_fps)*1000.0);

    movieHeight = pFormatCtx->streams[videoStream]->codec->height;
    movieWidth = pFormatCtx->streams[videoStream]->codec->width;

    // scale texture height and width to nerest power of two
    unsigned int two = 2;
    unsigned int heightPowerOfTwo = 0;
    while (movieHeight > (two<<heightPowerOfTwo))
        heightPowerOfTwo++;
    height = (two<<heightPowerOfTwo);
    unsigned int widthPowerOfTwo = 0;
    while (movieWidth > (two<<widthPowerOfTwo))
        widthPowerOfTwo++;
    width = (two<<widthPowerOfTwo);

    lineSize = width*numberOfChannels;
    data = new unsigned char[width*height*bytesPerColor*numberOfChannels];

    timeForTwoFrames = 2 * movie_spf;

    Restart();

    /*
    // Dump information about file onto standard error
    dump_format(pFormatCtx, 0, filename.c_str(), 0);
    logger.info << "texture size: " << width << "x" << height << logger.end;
    */
}

MovieResource::~MovieResource() {
    // Close the codec
    avcodec_close(pCodecCtx);

    // Close the video file
    av_close_input_file(pFormatCtx);

    delete[] data;
}

void MovieResource::Initialize() {
    // if texture id is not set, generate it
    if (id <= 0) {
        GLuint texid;
        glGenTextures(1, &texid);
        SetID(texid);
    }

    // decode the first frame
    DecodeOneFrame();
}

void MovieResource::Process(const float dt, const float percent) {
    if (pause || Ended()) return;
    time += dt; // @todo could overflow if to large

    float movieTime;
    while (time > (movieTime = frameNumber * movie_spf) ) { // decode all frames up to this point in time
        if (time > movieTime + timeForTwoFrames) { // if hardware is to slow to decode all the frames skip some
            int64_t seek_target = (int64_t)(time / movie_spf);
            if (pFormatCtx->streams[videoStream]->duration <= seek_target) // check not to seek outside the video
                seek_target = pFormatCtx->streams[videoStream]->duration-1;

            //logger.warning << "hardware is to slow, skipping frames in video: " << filename << logger.end;
            //logger.warning << "trying to skip from: " << frameNumber << " to: " << seek_target << logger.end;

            if (av_seek_frame(pFormatCtx, videoStream, seek_target, AVSEEK_FLAG_ANY) < 0)
                throw Exception("error while seeking for keeping up the speed, in file: " + filename);
            else
                frameNumber = seek_target;
        }
        DecodeOneFrame();
    }
}

void MovieResource::DecodeOneFrame() {
    AVPacket packet;

    int readIndex = -1;
    for (unsigned int countTryes = 0; readIndex != videoStream; countTryes++) {
        if (av_read_frame(pFormatCtx, &packet) >= 0)
            readIndex = packet.stream_index;
        else if (loop)
            Restart();
        else
            break;
        if (countTryes > pFormatCtx->nb_streams)
	  throw Exception("could not read frame, in file: " + filename + " tried " + Convert::int2string(countTryes) + " times");
    }

    // Allocate video frame
    AVFrame* pFrame = avcodec_alloc_frame();

    // Decode video frame
    int frameFinished;
    if (avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, packet.data, packet.size) < 0)
        throw Exception("could not decode, in file: " + filename);

    // Did we get a video frame?
    if (frameFinished) {
        AVPicture pict;
        pict.data[0] = data;
        pict.linesize[0] = lineSize;
        const int dst_pix_fmt = PIX_FMT_RGB32;

        //convertion of colors from yuv to rgba
	//--#ifdef _WIN32
        // from: http://www.ogre3d.org/phpBB2/viewtopic.php?t=29842&postdays=0&postorder=asc&start=25&sid=ce193664e1d3d7c4af509e6f4e2718c6	            
        if (!img_convert_ctx) {
	        img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                                         pCodecCtx->pix_fmt,
                                         pCodecCtx->width, pCodecCtx->height,
						                 dst_pix_fmt,
                                         SWS_BICUBIC, NULL, NULL, NULL);
        }
        sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pict.data, pict.linesize);

	/*--#else
        img_convert(&pict, dst_pix_fmt,
                    (AVPicture *)pFrame, pCodecCtx->pix_fmt, 
            pCodecCtx->width, pCodecCtx->height);
	    #endif
	--*/

        // fill the rest of the power of 2 texture with some color
        //avpicture_layout(&pict,PIX_FMT_RGB32, pCodecCtx->width ,pCodecCtx->height, data, width*height);

        // bind as OpenGL texture
        //glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_BGRA, GL_UNSIGNED_BYTE, data);

        frameNumber++;
    }
    /*else 
        logger.error << "decoding of frame did not finished" << logger.end;
    */
    // Free the YUV frame
    av_free(pFrame);
    // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);
}

void MovieResource::Restart() {
    time = 0.0;
    frameNumber = 0;
    if (av_seek_frame(pFormatCtx,0,0,0) < 0) //seek to the begining of the movie
        throw Exception("could not loop movie: " + filename);
    DecodeOneFrame(); // @todo: this is to avoid the error when decoding the first frame, where frameFinished becomes false
}

int MovieResource::GetMovieHeight() {
    return movieHeight;
}

int MovieResource::GetMovieWidth() {
    return movieWidth;
}

void MovieResource::Deinitialize() {
}

bool MovieResource::IsTypeOf(const std::type_info& inf) {
    return (typeid(MovieResource) == inf);
}

int MovieResource::GetID() {
    return id;
}

void MovieResource::SetID(int id) {
    this->id = id;
}

int MovieResource::GetHeight() {
    return height;
}

int MovieResource::GetWidth() {
    return width;
}

int MovieResource::GetDepth() {
    return numberOfChannels*8;
}

unsigned char* MovieResource::GetData() {
    return data;
}

void MovieResource::Load() {
}

void MovieResource::Unload() {
}

void MovieResource::Pause(bool pause) {
    this->pause = pause;
}

bool MovieResource::Ended() {
    return (frameNumber >= (pFormatCtx->streams[videoStream]->duration)-1);
}

} //NS Resources
} //NS OpenEngine
