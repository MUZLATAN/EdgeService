cmake_minimum_required(VERSION 3.0)



# add_definitions(-w) # close warning
include(cmake/${ARCH}.cmake)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)



set(CMAKE_VERBOSE_MAKEFILE  ON)
# enable_testing()
set(CMAKE_SYSTEM_NAME Linux)

#用于cout 输出__FILE__ 的相对路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILE__='\"$(subst  ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

project(vision-edge VERSION 1.0.0)

list(   APPEND
        REQUEST_LIBS
        pthread # folly depend on pthread, need put it down
        opencv_features2d
        opencv_video
        opencv_videoio #hisi平台用于本地读取视频测试
        opencv_imgcodecs
        opencv_imgproc
        opencv_core
        libjpeg-turbo
        libpng
        zlib
        dl
        jsoncpp
)

if (ARCH STREQUAL "x86")
        include_directories(${CMAKE_CURRENT_LIST_DIR}/dependencies/x86/jsoncpp/include
                            ${CMAKE_CURRENT_LIST_DIR}/dependencies/x86/opencv/include)
        link_directories(${CMAKE_CURRENT_LIST_DIR}/dependencies/x86/jsoncpp/lib
                         ${CMAKE_CURRENT_LIST_DIR}/dependencies/x86/opencv/lib)
        add_definitions("-D__PLATFORM_X86__")

elseif(ARCH STREQUAL "hisi")
        include_directories(${CMAKE_CURRENT_LIST_DIR}/dependencies/hisi/jsoncpp/include
                            ${CMAKE_CURRENT_LIST_DIR}/dependencies/hisi/opencv/include)
        link_directories(${CMAKE_CURRENT_LIST_DIR}/dependencies/hisi/jsoncpp/lib
                         ${CMAKE_CURRENT_LIST_DIR}/dependencies/hisi/opencv/lib
                         ${CMAKE_CURRENT_LIST_DIR}/dependencies/hisi/ffmpeg/lib
                         ${CMAKE_CURRENT_LIST_DIR}/dependencies/hisi/opencv/share/OpenCV/3rdparty/lib)

        list(APPEND THIRD_LIBS opencv_videoio avcodec avfilter avformat avcodec avutil swresample )

        add_definitions("-D__PLATFORM_HISI__")
        message(STATUS "HERE !!!" )
endif()





#编译源码
include_directories(${CMAKE_SOURCE_DIR}/include)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/ META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/event META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/network META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/node META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/mgr META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/app META_SRC)


# 编译modules
# add_subdirectory(${CMAKE_SOURCE_DIR}/modules/)
# include_directories(${CMAKE_SOURCE_DIR}/modules/infer_engine/include)
# list(APPEND MODULE_LIBS infer_engine)


set(BIN META)
add_executable(${BIN} ${META_SRC})
target_link_libraries(${BIN} ${REQUEST_LIBS} ${MODULE_LIBS} ${THIRD_LIBS})

