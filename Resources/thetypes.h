#ifndef THETYPES
#define THETYPES

#ifdef _WIN32
#include <avcodec.h>
#include <avformat.h>
#else
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>
#endif

#include <Meta/SDL.h>

#define VIDEO_PICTURE_QUEUE_SIZE 1

typedef struct VideoPicture {
  SDL_Overlay *bmp;
  int width, height; /* source height & width */
  int allocated;
  double pts;
} VideoPicture;

typedef struct VideoState {
  AVFormatContext *pFormatCtx;
  int             videoStream, audioStream;

  int             av_sync_type;
  double          external_clock; /* external clock base */
  int64_t         external_clock_time;
  int             seek_req;
  int             seek_flags;
  int64_t         seek_pos;

  double          audio_clock;
  AVStream        *audio_st;
  //CPVC PacketQueue     audioq;
  uint8_t         audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
  unsigned int    audio_buf_size;
  unsigned int    audio_buf_index;
  AVPacket        audio_pkt;
  uint8_t         *audio_pkt_data;
  int             audio_pkt_size;
  int             audio_hw_buf_size;  
  double          audio_diff_cum; /* used for AV difference average computation */
  double          audio_diff_avg_coef;
  double          audio_diff_threshold;
  int             audio_diff_avg_count;
  double          frame_timer;
  double          frame_last_pts;
  double          frame_last_delay;
  double          video_clock; ///<pts of last decoded frame / predicted pts of next decoded frame
  double          video_current_pts; ///<current displayed pts (different from video_clock if frame fifos are used)
  int64_t         video_current_pts_time;  ///<time (av_gettime) at which we updated video_current_pts - used to have running video pts
  AVStream        *video_st;
  //CPVC PacketQueue     videoq;
  VideoPicture    pictq[VIDEO_PICTURE_QUEUE_SIZE];
  int             pictq_size, pictq_rindex, pictq_windex;
  SDL_mutex       *pictq_mutex;
  SDL_cond        *pictq_cond;
  SDL_Thread      *parse_tid;
  SDL_Thread      *video_tid;

  char            filename[1024];
  int             quit;
} VideoState;

enum {
  AV_SYNC_AUDIO_MASTER,
  AV_SYNC_VIDEO_MASTER,
  AV_SYNC_EXTERNAL_MASTER,
};

#endif
