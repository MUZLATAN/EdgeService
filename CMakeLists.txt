cmake_minimum_required(VERSION 3.0)
project(vision-edge VERSION 1.0.0)

# add_definitions(-w) # close warning

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_VERBOSE_MAKEFILE  ON)
enable_testing()
set(CMAKE_SYSTEM_NAME Linux)

#用于cout 输出__FILE__ 的相对路径
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILE__='\"$(subst  ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")



# add_compile_options(-fPIC)
option(WITH_HEAPPROFILE " build with heap profile " OFF)
option(WITH_GTESTS "build unit test " OFF)

list(
        APPEND
        REQUEST_LIBS
        pthread # folly depend on pthread, need put it down
        opencv_videoio
        opencv_video
        opencv_core
        opencv_imgproc
        opencv_highgui
        opencv_imgcodecs
        jsoncpp

)





 include_directories(${CMAKE_CURRENT_LIST_DIR}/dependencies/x86/include/)
 link_directories(${CMAKE_CURRENT_LIST_DIR}/dependencies/x86/lib)

add_definitions("-D__PLATFORM_X86__")

message(STATUS "NOTE: PLATFORM is: " ${PLATFORM})

include_directories(${CMAKE_SOURCE_DIR}/include)

aux_source_directory(${CMAKE_SOURCE_DIR}/src/ META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/event META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/network META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/node META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/mgr META_SRC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/app META_SRC)

set(BIN META)
add_executable(${BIN} ${META_SRC})
target_link_libraries(${BIN} ${REQUEST_LIBS})