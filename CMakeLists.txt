SET( EXTENSION_NAME "Extensions_FFMPEGResource")

ADD_LIBRARY( ${EXTENSION_NAME}
  Resources/IMovieResource.h
  Resources/FFMPEGResource.h
  Resources/FFMPEGResource.cpp
)

TARGET_LINK_LIBRARIES( ${EXTENSION_NAME}
  OpenEngine_Core
  OpenEngine_Resources
  ${FFMPEG_LIBRARIES}
  # Extension dependencies
  Extensions_GenericHandlers
)
