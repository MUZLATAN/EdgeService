cmake_minimum_required(VERSION 3.0)
project(vision-edge VERSION 1.0.0)

# add_definitions(-w) # close warning

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_VERBOSE_MAKEFILE  ON)
enable_testing()
set(CMAKE_SYSTEM_NAME Linux)

find_library(BOOSTTEST_LIBRARY NAMES boost_unit_test_framework)

# add_compile_options(-fPIC)
option(WITH_HEAPPROFILE " build with heap profile " OFF)
option(WITH_GTESTS "build unit test " OFF)

list(
        APPEND
        REQUEST_LIBS
        gflags
        glog
        boost_context
        unwind
        lzma
        iberty
        pthread # folly depend on pthread, need put it down
        opencv_core
        opencv_imgproc
        opencv_highgui
        opencv_imgcodecs
        opencv_videoio
        opencv_video
        opencv_calib3d
        jsoncpp
        x264
)

#include_directories(/usr/local/include/)
#include_directories(/usr/include /usr/include/opencv4 /usr/include/jsoncpp/)
#link_directories(/usr/local/lib /usr/lib)

 set(ALGO_3RDPARTY_PATH /env/platforms/x86_64)

 message(STATUS "NOTE: ALGO_3RDPARTY_PATH is: " ${ALGO_3RDPARTY_PATH})
 include_directories(${ALGO_3RDPARTY_PATH}/include/)
 link_directories(${ALGO_3RDPARTY_PATH}/lib
         ${ALGO_3RDPARTY_PATH}/static
         ${ALGO_3RDPARTY_PATH}/so )

add_definitions("-D__PLATFORM_X86__")

message(STATUS "NOTE: PLATFORM is: " ${PLATFORM})

include_directories(${CMAKE_SOURCE_DIR}/include)

aux_source_directory(${CMAKE_SOURCE_DIR}/src/ ALGO_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/event ALGO_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/network ALGO_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/node ALGO_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/mgr ALGO_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/app ALGO_SRC)

set(BIN ALGO)
add_executable(${BIN} ${ALGO_SRC})
target_link_libraries(${BIN} ${REQUEST_LIBS})