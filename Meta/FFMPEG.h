#ifndef _META_FFMPEG_
#define _META_FFMPEG_

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

#endif // _META_FFMPEG_
