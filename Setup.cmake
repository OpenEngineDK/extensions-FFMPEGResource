INCLUDE(extensions/MovieResource/FindFFMPEG.cmake)

IF (FFMPEG_FOUND)
  INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIRS})
ELSE (FFMPEG_FOUND)
  MESSAGE ("WARNING: Could not find FFMPEG - depending targets will be disabled.")
ENDIF (FFMPEG_FOUND)
