# - Try to find FFMPEG
# Once done this will define
#
#  FFMPEG_FOUND - system has FFMPEG
#  FFMPEG_INCLUDE_DIRS - the FFMPEG include directory
#  FFMPEG_LIBRARIES - Link these to use FFMPEG
#  FFMPEG_DEFINITIONS - Compiler switches required for using FFMPEG
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


#if (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)
  # in cache already
#  set(FFMPEG_FOUND TRUE)
#else (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  include(UsePkgConfig)

  pkgconfig(libavcodec _FFMPEGIncDir _FFMPEGLinkDir _FFMPEGLinkFlags _FFMPEGCflags)

  set(FFMPEG_DEFINITIONS ${_FFMPEGCflags})

  find_path(FFMPEG_INCLUDE_DIR
    NAMES
      avcodec.h
    PATHS
      ${_FFMPEGIncDir}
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      ${PROJECT_SOURCE_DIR}/libraries/ffmpeg/include
    PATH_SUFFIXES
      ffmpeg
  )

  if (NOT APPLE)
    find_library(AVUTIL_LIBRARY
      NAMES
        avutil
      PATHS
        ${_FFMPEGLinkDir}
        /usr/lib
        /usr/local/lib
        /opt/local/lib
        /sw/lib
    )
    find_library(AVUTIL_LIBRARY
      NAMES
        avutil-49
      PATHS
        ${PROJECT_SOURCE_DIR}/libraries/ffmpeg/lib
    )
  endif (NOT APPLE)

  find_library(AVCODEC_LIBRARY
    NAMES
      avcodec
    PATHS
      ${_FFMPEGLinkDir}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )
  find_library(AVCODEC_LIBRARY
    NAMES
      avcodec-51
    PATHS
      ${PROJECT_SOURCE_DIR}/libraries/ffmpeg/lib
  )

  find_library(AVFORMAT_LIBRARY
    NAMES
      avformat
    PATHS
      ${_FFMPEGLinkDir}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
  )
  find_library(AVFORMAT_LIBRARY
    NAMES
      avformat-51
    PATHS
      ${PROJECT_SOURCE_DIR}/libraries/ffmpeg/lib
  )

  find_library(SWSCALE_LIBRARY
    NAMES
      swscale-0
    PATHS
      ${PROJECT_SOURCE_DIR}/libraries/ffmpeg/lib
  )

  set(FFMPEG_INCLUDE_DIRS
    ${FFMPEG_INCLUDE_DIR}
  )
 
  if (WIN32)
  set(FFMPEG_LIBRARIES
    ${AVUTIL_LIBRARY}
    ${AVCODEC_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${SWSCALE_LIBRARY}
  )
  else(WIN32)
  set(FFMPEG_LIBRARIES
    ${AVUTIL_LIBRARY}
    ${AVCODEC_LIBRARY}
    ${AVFORMAT_LIBRARY}
  )
  endif(WIN32)

  if (FFMPEG_INCLUDE_DIRS AND FFMPEG_LIBRARIES)
     set(FFMPEG_FOUND TRUE)
  endif (FFMPEG_INCLUDE_DIRS AND FFMPEG_LIBRARIES)

  if (FFMPEG_FOUND)
    if (NOT FFMPEG_FIND_QUIETLY)
      #message(STATUS "Found FFMPEG: ${FFMPEG_LIBRARIES}")
    endif (NOT FFMPEG_FIND_QUIETLY)
  else (FFMPEG_FOUND)
    if (FFMPEG_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find FFMPEG")
    endif (FFMPEG_FIND_REQUIRED)
  endif (FFMPEG_FOUND)

  # show the FFMPEG_INCLUDE_DIRS and FFMPEG_LIBRARIES variables only in the advanced view
  mark_as_advanced(FFMPEG_INCLUDE_DIRS FFMPEG_LIBRARIES)

#endif (FFMPEG_LIBRARIES AND FFMPEG_INCLUDE_DIRS)

