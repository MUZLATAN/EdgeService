cmake_minimum_required(VERSION 3.0.0)
project(infer_engine)

message(STATUS "======== BUILD infer_engine ========")
message(STATUS "ARCH: ${ARCH}")

# ================ 三方库 ================
# OpenCV
list(
    APPEND
    OPENCV_LIBS
    opencv_core
    opencv_imgproc
)

# 后端
set(BACKEND_LIB "")
if (${ARCH} STREQUAL "x86" OR ${ARCH} STREQUAL "rk")
    set(BACKEND_LIB "tengine-lite-static")
elseif(${ARCH} STREQUAL "hisi")
    list(APPEND BACKEND_LIB 
    mpi
    ive
    md
    nnie
    hdmi
    _hiae
    isp
    _hidehaze
    _hidrc
    _hildci
    _hiawb
    sns_imx327
    sns_imx327_2l
    sns_imx307
    sns_imx458
    sns_mn34220
    sns_os05a
    VoiceEngine
    securec
    sns_imx335
    upvqe
    dnvqe)
elseif(${ARCH} STREQUAL "rv")
    set(BACKEND_LIB "rknn_api")
endif()

# 编译库
include_directories(include)
aux_source_directory(src SRCS)
aux_source_directory(src/engine SRCS)

add_library(infer_engine ${SRCS})
target_link_libraries(infer_engine
    ${OPENCV_LIBS}
    ${BACKEND_LIB}
)