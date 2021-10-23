#!/bin/bash

# 要编译的平台: (默认x86)
##    x86
PLATFORM="x86"

# 当前目录
CUR_PATH=`pwd`
# 输出平台目录
x86_LIB_PATH="x86"

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

build_opencv()
{
    cd ${SRC_PATH}/opencv
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build
    cd build
#    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/opencv ${COMPILER} -DAR=${AR} -DRANLIB=${RANLIB} -DBUILD_LIST="core,imgproc,highgui,imgcodecs,videoio,video,calib3d,features2d" -DBUILD_SHARED_LIBS=OFF -DBUILD_opencv_apps=OFF -DOPENCV_FORCE_3RDPARTY_BUILD=OFF -DWITH_GTK=OFF -DWITH_IPP=OFF -DBUILD_TESTS=OFF -DWITH_1394=OFF -DBUILD_opencv_apps=OFF -DWITH_ITT=OFF -DBUILD_ZLIB=ON -DWITH_TIFF=OFF -DWITH_JASPER=OFF -DWITH_OPENEXR=OFF -DWITH_WEBP=OFF -DCMAKE_CXX_FLAGS="-DCVAPI_EXPORTS" ..
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/opencv ${COMPILER}  -DBUILD_LIST="core,imgproc,highgui,imgcodecs,videoio,video,calib3d,features2d" -DBUILD_SHARED_LIBS=ON -DBUILD_TESTS=OFF -DWITH_IPP=OFF ..
    make -j2
    make install


    cp -r ${INSTALL_DIR}/opencv/include/* ${INSTALL_DIR}/include
    cp -r ${INSTALL_DIR}/opencv/lib/* ${INSTALL_DIR}/lib
    rm -rf ${INSTALL_DIR}/opencv
    cd ${CUR_PATH}
}



# ================ Tengine ================
build_tengine()
{
    cd ${SRC_PATH}/tengine
    mkdir -p build 
    cd ./build
    cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/aarch64-linux-gnu.toolchain.cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/tengine -DTENGINE_BUILD_BENCHMARK=OFF -DTENGINE_BUILD_EXAMPLES=OFF -DCMAKE_BUILD_TYPE=release ${COMPILER} ..
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

setup
download_all
build_for_x86

#rm -rf src

