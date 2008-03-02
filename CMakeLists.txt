
ADD_LIBRARY(Extensions_MovieResource
  Resources/MovieResource.cpp
)

TARGET_LINK_LIBRARIES(Extensions_MovieResource
  OpenEngine_Resources
  ${FFMPEG_LIBRARIES}
  # Extension dependencies
  Extensions_GenericHandlers
)
