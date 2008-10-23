#ifndef _META_FFMPEG_
#define _META_FFMPEG_

#include <Meta/Types.h>
//#include <Meta/SDL.h> // only to include anti C types like uint8

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
extern "C" {
    #include <ffmpeg/avcodec.h>
    #include <ffmpeg/avformat.h>
    #include <ffmpeg/swscale.h>
}
#endif
#endif

#endif // _META_FFMPEG_
