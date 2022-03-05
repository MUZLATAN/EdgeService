cmake_minimum_required(VERSION 3.0)
project(vision-api VERSION 1.0.0)

# add_definitions(-w) # close warning specify the C++ standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_SYSTEM_NAME linux)
enable_testing()

list(
  APPEND
  opencv_core
  opencv_highgui
  opencv_imgproc
  opencv_imgcodecs
  # opencv_videoio
  opencv_calib3d
  opencv_features2d)

if( ${ARCH} STREQUAL "hisi")
  message(STATUS "hello" ${ARCH})
endif()


add_definitions("-O3")

# 依赖infer_engine
add_subdirectory(./infer_engine)

# if(ARCH STREQUAL "hisi")
#   add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/hisi)
# endif()
