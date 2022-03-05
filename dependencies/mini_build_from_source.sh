#!/bin/bash

# 要编译的平台: (默认x86)
##    x86
PLATFORM="x86"

# 当前目录
CUR_PATH=`pwd`
# 输出平台目录
x86_LIB_PATH="x86"
HISI_LIB_PATH="hisi"

# 源码存放目录
SRC_PATH="${CUR_PATH}/src"


##############################
#        三方库配置           #
##############################

# 版本，默认master
declare -A LIB_VERSIONS=( 
    ["opencv"]="3.4"
)

# 地址
declare -A LIB_URLS=( 
    ["opencv"]="https://gitee.com/mirrors/opencv.git"
    ["ffmpeg"]="https://ffmpeg.org/releases/ffmpeg-3.4.9.tar.gz"
    # ["rknn"]="https://codeload.github.com/rockchip-linux/rknpu/zip/refs/heads/master"
    ["jsoncpp"]="https://codeload.github.com/open-source-parsers/jsoncpp/tar.gz/refs/tags/1.8.3"
)


##########################################
#                  函数                  #
##########################################
# 新建目录
# Param: 目录名称
makedir()
{
    if ! [ -d $1 ];
    then
        mkdir -p $1
    fi
}

# 初始化
setup()
{
    export GIT_SSL_NO_VERIFY=1

    makedir "$SRC_PATH"

    if [ $PLATFORM = "x86" ];
    then
        makedir "${x86_LIB_PATH}"
    fi        
    
    if [ $PLATFORM = "hisi"]
    then
        makedir "${LIB_PATH}/${HISI_LIB_PATH}"
    fi

    apt-get install -y libtool
}

# 下载源码
# Param: 
#     库名称
download()
{
    url=${LIB_URLS[$1]}
    download_method=""
    if [[ "$url" == *.git ]]; then
        download_method="git"
    else
        download_method="wget"
    fi
    if [[ "${download_method}" == "git" ]]; then
        default_lib_ver="master"
        ver=""
        if [ -z "${LIB_VERSIONS[$1]}" ]; then
            ver=${default_lib_ver}
        else
            ver=${LIB_VERSIONS[$1]}
        fi
        if ! [ -d $1 ];
        then
            git clone -b ${ver} --recursive ${LIB_URLS[$1]} $1 --depth=1
        else
            cd $1
            git checkout ${ver}
            cd ..
        fi
    else
        tar_name=$(basename "${LIB_URLS[$1]}")
        if ! [ -f ${tar_name} ]; then
            wget -c --no-check-certificate ${LIB_URLS[$1]}
        fi
        if ! [ -d $1 ]; then
            mkdir $1
            tar zxf ${tar_name} -C $1 --strip-components=1
        fi
    fi
}

# 下载所有源码
download_all()
{
    cd ${SRC_PATH}
    for lib in ${!LIB_URLS[@]}; do
        echo "================ Download Step for ${lib} ================="
        download "${lib}"
        status_code=$?
        if ! [ ${status_code} -eq 0 ]; then
            exit 1
        fi
    done
    cd ..
}
build_rknn()
{
    echo "================ Build rknn ================"
    cd ${SRC_PATH}
    cp -rf ./rknn ${INSTALL_DIR}/
    cd ${CUR_PATH}
}
# ================ FFMpeg ================
build_ffmpeg()
{
    cd ${SRC_PATH}/ffmpeg
    if [[ ${BUILD_PLATFORM} == "rk" ]];
    then
        cd ${SRC_PATH}
        if [ -d ffmpeg ];
        then
            rm -r ffmpeg
        fi

        wget -c https://whale-cv-video.oss-cn-hangzhou.aliyuncs.com/ffmpeg_rkmpp.tar.gz
        tar zxf ffmpeg_rkmpp.tar.gz
        cp -rf ffmpeg ${INSTALL_DIR}

        FFMEPG_LIB_PATH=${INSTALL_DIR}/ffmpeg/lib
        FFMPEG_PKG_PATH=${INSTALL_DIR}/ffmpeg/lib/pkgconfig
    elif [[ ${BUILD_PLATFORM} == "hisi" ]];
    then
        ./configure --prefix=${INSTALL_DIR}/ffmpeg --pkg-config=pkg-config --enable-version3 --enable-ffmpeg --enable-static --enable-pic --disable-ffplay --disable-ffprobe --disable-doc --disable-htmlpages --disable-podpages --disable-txtpages --cross-prefix=${COMPILER_PREFIX} --target-os=linux --enable-cross-compile --arch=arm --enable-nonfree --enable-gpl --enable-lto
        make -j$(nproc)
        make install
        FFMEPG_LIB_PATH=${INSTALL_DIR}/ffmpeg/lib
        FFMPEG_PKG_PATH=${INSTALL_DIR}/ffmpeg/lib/pkgconfig
    else
        export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${INSTALL_DIR}/x264/lib/pkgconfig:${INSTALL_DIR}/x265/lib/pkgconfig:${INSTALL_DIR}/rk_mpp/lib/pkgconfig"
        ./configure --enable-version3 --enable-gpl --enable-rkmpp --enable-nonfree --enable-libx264  --enable-ffmpeg --enable-static --enable-pic --enable-lto --disable-ffplay --disable-ffprobe --disable-doc --disable-htmlpages --disable-podpages --disable-txtpages --cross-prefix=${COMPILER_PREFIX} --target-os=linux --enable-cross-compile --arch=arm --enable-nonfree --enable-gpl --extra-cflags="-I${INSTALL_DIR}/x264/include -I${INSTALL_DIR}/x265/include -I${INSTALL_DIR}/rk_mpp/include -I/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/arm-linux-gnueabihf/libc/usr/include/drm" \
        --extra-ldflags="-L${INSTALL_DIR}/x264/lib -L${INSTALL_DIR}/x265/lib -L${INSTALL_DIR}/rk_mpp/lib -L/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/arm-linux-gnueabihf/libc/usr/lib"
        make -j$(nproc)
        make install
        FFMEPG_LIB_PATH=${INSTALL_DIR}/ffmpeg/lib
        FFMPEG_PKG_PATH=${INSTALL_DIR}/ffmpeg/lib/pkgconfig
    fi
    
    cd ${CUR_PATH}
}

build_jsoncpp()
{
    
    cd ${SRC_PATH}/
    tar -zvxf 1.8.3 
    cd jsoncpp-1.8.3
    sed -i "5s/ENABLE_TESTING()/# ENABLE_TESTING()/g" ./CMakeLists.txt
    sed -i "7s/ ON/ OFF/g" ./CMakeLists.txt
    sed -i "8s/ ON/ OFF/g" ./CMakeLists.txt
    sed -i "53s/std::snprintf/snprintf/g" ./src/lib_json/json_writer.cpp
    sed -i "61s/std::snprintf/snprintf/g" ./src/lib_json/json_writer.cpp
    sed -i "27s/std::snprintf/snprintf/g" ./src/lib_json/json_reader.cpp
    sed -i "35s/std::snprintf/snprintf/g" ./src/lib_json/json_reader.cpp

    mkdir -p build
    cd ./build

    cmake ${COMPILER}                                   \
        -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/jsoncpp   \
    ..

    make -j$(nproc) & make install
    cd ${CUR_PATH}
}


# 构建: RK
build_for_x86()
{
    echo "================ Build for x86 ================"
    INSTALL_DIR="${CUR_PATH}/${x86_LIB_PATH}"
    C_COMPILER="gcc"
    CXX_COMPILER="c++"
    LD="ld"
    AR="ar"
    RANLIB="ranlib"
    COMPILER="-DCMAKE_C_COMPILER=${C_COMPILER} -DCMAKE_CXX_COMPILER=${CXX_COMPILER}"
    BUILD_PLATFORM="rk"
    HOST_SYSTEM_NAME="linux"
    CONFIGURE_HOST="linux"
    TOOLCHAIN_ROOT="$(dirname `which ${C_COMPILER}`)/../"
    TOOLCHAIN_INCLUDE="${TOOLCHAIN_ROOT}/include"
    TOOLCHAIN_LIB="${TOOLCHAIN_ROOT}/lib"

    makedir ${INSTALL_DIR}/include
    makedir ${INSTALL_DIR}/lib

    export LD_LIBRARY_PATH=
    build_opencv
    echo "================ Build for x86 DONE ================"
}

# 构建: RV
build_for_rv()
{
    echo "================ Build for RV ================"
    INSTALL_DIR="${CUR_PATH}/${LIB_PATH}/${RV_LIB_PATH}"
    COMPILER_PREFIX="arm-linux-gnueabihf-"
    C_COMPILER="${COMPILER_PREFIX}gcc"
    CXX_COMPILER="${COMPILER_PREFIX}c++"
    CC=${COMPILER_PREFIX}gcc 
    CXX=${COMPILER_PREFIX}g++
    LD="${COMPILER_PREFIX}ld"
    AR="${COMPILER_PREFIX}ar"
    RANLIB="${COMPILER_PREFIX}ranlib"
    COMPILER="-DCMAKE_C_COMPILER=${C_COMPILER} -DCMAKE_CXX_COMPILER=${CXX_COMPILER} -DRANDLIB=${RANDLIB}"
    BUILD_PLATFORM="rv"
    HOST_SYSTEM_NAME="linux-generic32"
    CONFIGURE_HOST="arm-none-linux"
    TOOLCHAIN_ROOT="$(dirname `which ${C_COMPILER}`)/../"
    TOOLCHAIN_INCLUDE="${TOOLCHAIN_ROOT}/include"
    TOOLCHAIN_LIB="${TOOLCHAIN_ROOT}/lib"
    CPU="cortex-a7"
    FPU="neon"
    CMAKE_SYSTEM_PROCESSOR="armv7-a_hardfp"

    export LD_LIBRARY_PATH=

    rm -rf ${SRC_PATH}
    # download_all
    

    build_opencv
    # build_rknn
    # build_jsoncpp

    echo "================ Build for RV DONE ================"
}

# 构建：海思
build_for_hisi()
{
    echo "================ Build for hisi ================"
    INSTALL_DIR="${CUR_PATH}/${LIB_PATH}/${HISI_LIB_PATH}/${BUILD_TYPE}"
    COMPILER_PREFIX="arm-himix200-linux-"
    C_COMPILER="${COMPILER_PREFIX}gcc"
    CXX_COMPILER="${COMPILER_PREFIX}g++"
    LD="${COMPILER_PREFIX}ld"
    AR="${COMPILER_PREFIX}ar"
    RANDLIB="${COMPILER_PREFIX}randlib"
    COMPILER="-DCMAKE_C_COMPILER=${C_COMPILER} -DCMAKE_CXX_COMPILER=${CXX_COMPILER} -DRANDLIB=${RANDLIB}"
    BUILD_PLATFORM="hisi"
    HOST_SYSTEM_NAME="linux-generic32"
    CONFIGURE_HOST="arm-none-linux"
    TOOLCHAIN_ROOT="$(dirname `which ${C_COMPILER}`)/../arm-linux-gnueabi"
    TOOLCHAIN_INCLUDE="${TOOLCHAIN_ROOT}/include"
    TOOLCHAIN_LIB="${TOOLCHAIN_ROOT}/lib"
    CPU="cortex-a7"
    FPU="neon"

    export LD_LIBRARY_PATH=
    download_all
    build_ffmpeg
    build_opencv
    # build_rknn
    # build_jsoncpp


    echo "================ Build for hisi DONE ================"
}

setup
# download_all
# build_for_x86
build_for_hisi

#rm -rf src

