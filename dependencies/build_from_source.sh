#!/bin/bash

# 要编译的平台: (默认全平台)
##    x86
PLATFORM="all"

# 当前目录
CUR_PATH=`pwd`
# 输出根目录
LIB_PATH="thirdparty"
# 输出平台目录
HISI_LIB_PATH="hisi"
RK_LIB_PATH="rk"
x86_LIB_PATH="x86"

# 源码存放目录
SRC_PATH="${CUR_PATH}/thirdparty_src"


##############################
#        三方库配置           #
##############################

# 版本，默认master
declare -A LIB_VERSIONS=( 
    ["opencv"]="3.4" 
    ["boost"]="1.67.0"
    ["folly"]="whale"
    ["mxml"]="release-2.8"
    ["rk_mpp"]="release"
    ["tengine"]="tengine-lite"
    ["libcurl"]="curl-7_78_0"
)

# 地址
boost__v=${LIB_VERSIONS[boost]}
boost__v=${boost__v//./_}
declare -A LIB_URLS=( 
    ["opencv"]="https://gitee.com/mirrors/opencv.git" 
    ["boost"]="https://boostorg.jfrog.io/artifactory/main/release/${LIB_VERSIONS[boost]}/source/boost_${boost__v}.tar.gz"
    ["fmt"]="https://gitee.com/creatw/fmt.git"
    ["uuid"]="https://github.com/boostorg/uuid/archive/refs/tags/boost-${LIB_VERSIONS[boost]}.tar.gz"
    ["gtest"]="https://github.com/google/googletest/archive/release-1.8.0.tar.gz"
    ["double-conversion"]="https://github.com/google/double-conversion/archive/refs/tags/v3.1.5.tar.gz"
    ["gflags"]="https://github.com/gflags/gflags/archive/refs/tags/v2.2.2.tar.gz"
    ["glog"]="https://gitee.com/mirrors/glog.git"
    ["openssl"]="https://www.openssl.org/source/old/1.1.0/openssl-1.1.0l.tar.gz"
    ["libevent"]="https://gitee.com/mirrors/libevent.git"
    ["folly"]="https://gitee.com/ml-inory/folly.git"
    ["zlib"]="https://gitee.com/mirrors/zlib.git"
    ["cpprest"]="https://gitee.com/mirrors/cpprestsdk.git"
    ["ffmpeg"]="https://ffmpeg.org/releases/ffmpeg-4.1.tar.gz"
    ["freetype"]="https://gitee.com/ml-inory/freetype_sdl.git"
    ["hisi_mpp"]="https://gitee.com/ml-inory/hisi_mpp.git"
    ["mosquitto"]="https://github.com/eclipse/mosquitto.git"
    ["libcurl"]="https://gitee.com/mirrors/curl.git"
    ["apr_host"]="https://mirrors.bfsu.edu.cn/apache//apr/apr-1.7.0.tar.gz"
    ["apr"]="https://mirrors.bfsu.edu.cn/apache//apr/apr-1.7.0.tar.gz"
    ["apr-util"]="https://archive.apache.org/dist/apr/apr-util-1.5.4.tar.gz"
    ["mxml"]="https://github.com/michaelrsweet/mxml/archive/refs/tags/${LIB_VERSIONS[mxml]}.tar.gz"
    ["oss_c_sdk"]="http://docs-aliyun.cn-hangzhou.oss.aliyun-inc.com/assets/attach/32131/cn_zh/1501595738954/aliyun-oss-c-sdk-3.5.0.tar.gz?spm=a2c4g.11186623.2.11.583d26fdd135jG&file=aliyun-oss-c-sdk-3.5.0.tar.gz"
    ["rk_mpp"]="https://github.com/rockchip-linux/mpp/archive/refs/tags/release_20171218.tar.gz"
    ["tengine"]="https://github.com/OAID/Tengine/archive/refs/tags/lite-v1.5.tar.gz"
    ["x264"]="https://code.videolan.org/videolan/x264.git"
    ["hisi_runtime"]="https://gitee.com/ml-inory/hisi_runtime.git"
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
    
    if [ -d ${LIB_PATH} ]; then
        rm -rf ${LIB_PATH}
    fi
    makedir "$LIB_PATH"
    makedir "$SRC_PATH"

    if [ $PLATFORM = "all" ];
    then
        makedir "${LIB_PATH}/${HISI_LIB_PATH}"
        makedir "${LIB_PATH}/${RK_LIB_PATH}"
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
    
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/opencv ${COMPILER} -DAR=${AR} -DRANLIB=${RANLIB} -DBUILD_LIST="core,imgproc,highgui,imgcodecs,videoio,video,calib3d,features2d" -DBUILD_SHARED_LIBS=OFF -DBUILD_opencv_apps=OFF -DOPENCV_FORCE_3RDPARTY_BUILD=OFF -DWITH_GTK=OFF -DWITH_IPP=OFF -DBUILD_TESTS=OFF -DWITH_1394=OFF -DBUILD_opencv_apps=OFF -DWITH_ITT=OFF -DBUILD_ZLIB=ON -DWITH_TIFF=OFF -DWITH_JASPER=OFF -DWITH_OPENEXR=OFF -DWITH_WEBP=OFF -DCMAKE_CXX_FLAGS="-DCVAPI_EXPORTS" ..
    make -j$(nproc)
    make install
    cp 3rdparty/lib/* ${INSTALL_DIR}/opencv/lib
    cd ${CUR_PATH}
}


# ================ Folly ================
build_fmt()
{
    cd ${SRC_PATH}/fmt
    if [ -d build ]; then
        rm -rf build
    fi

    mkdir -p build
    cd ./build
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/fmt -DFMT_DOC=OFF -DFMT_TEST=OFF ${COMPILER} ..
    make -j$(nproc) & make install
    LIBFMT_INCLUDE_DIR="${INSTALL_DIR}/fmt/include"
    LIBFMT_LIBRARY_DIR="${INSTALL_DIR}/fmt/lib"
    cd ${CUR_PATH}
}

build_boost()
{
    cd ${SRC_PATH}/boost
    ./bootstrap.sh --without-libraries=python
    sed -i "12s%using gcc%using gcc : ${BUILD_PLATFORM} : ${C_COMPILER}%g" project-config.jam
    ./b2 link=static runtime-link=static target-os=linux architecture=arm abi=aapcs binary-format=elf  --prefix=${INSTALL_DIR}/boost install
    BOOST_INCLUDE_DIR="${INSTALL_DIR}/boost/include"
    BOOST_LIBRARY_DIR="${INSTALL_DIR}/boost/lib"
    cd ${CUR_PATH}
}

build_uuid()
{
    BOOST_INCLUDE_DIR="${INSTALL_DIR}/boost/include"
    cd ${SRC_PATH}/uuid
    cp -rf include/boost/uuid ${BOOST_INCLUDE_DIR}/boost
    cd ${CUR_PATH}
}

build_gtest()
{
    cd ${SRC_PATH}/gtest
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build
    cd ./build

    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/gtest ${COMPILER} -DBUILD_GTEST=ON ..
    make -j$(nproc) & make install
    GTEST_INCLUDE_DIR="${INSTALL_DIR}/gtest/include"
    GTEST_LIBRARY_DIR="${INSTALL_DIR}/gtest/lib"
    cd ${CUR_PATH}
}

build_double-conversion()
{
    cd ${SRC_PATH}/double-conversion
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build 
    cd ./build

    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/double-conversion ${COMPILER} ..
    make -j$(nproc) & make install
    DOUBLE_CONVERSION_INCLUDE_DIR="${INSTALL_DIR}/double-conversion/include"
    DOUBLE_CONVERSION_LIBRARY_DIR="${INSTALL_DIR}/double-conversion/lib"
    cd ${CUR_PATH}
}

build_gflags()
{
    cd ${SRC_PATH}/gflags
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build 
    cd ./build
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/gflags -DBUILD_SHARED_LIBS=OFF -DBUILD_TESTING=OFF ${COMPILER} ..
    make -j$(nproc) & make install
    GFLAGS_INCLUDE_DIR="${INSTALL_DIR}/gflags/include"
    GFLAGS_LIBRARY_DIR="${INSTALL_DIR}/gflags/lib"
    cd ${CUR_PATH}
}

build_glog()
{
    cd ${SRC_PATH}/glog
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build
    cd ./build
    cmake -DBUILD_SHARED_LIBS=OFF -DWITH_PKGCONFIG=OFF -DWITH_SYMBOLIZE=OFF -DWITH_UNWIND=OFF -DGFLAGS_INCLUDE_DIR=${GFLAGS_INCLUDE_DIR} -DGFLAGS_LIBRARIES="${GFLAGS_LIBRARY_DIR}/libgflags.a;${GFLAGS_LIBRARY_DIR}/libgflags_nothreads.a" -DGTEST_INCLUDE_DIR=${GTEST_INCLUDE_DIR} -DGTEST_LIBRARY=${GTEST_LIBRARY_DIR}/libgtest.a -DGTEST_MAIN_LIBRARY=${GTEST_LIBRARY_DIR}/libgtest_main.a -DCMAKE_PREFIX_PATH="${GFLAGS_LIBRARY_DIR};${GTEST_LIBRARY_DIR}" -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/glog ${COMPILER} -DCMAKE_CXX_FLAGS="-fPIC" -DBUILD_TESTING=OFF ..
    make -j$(nproc) & make install
    GLOG_INCLUDE_DIR="${INSTALL_DIR}/glog/include"
    GLOG_LIBRARY_DIR="${INSTALL_DIR}/glog/lib"
    cd ${CUR_PATH}
}

build_openssl()
{
    cd ${SRC_PATH}/openssl
    if [ -f libcrypto.a ]; then
        make clean
    fi
    export LD_LIBRARY_PATH=
    OPENSSL_ROOT_DIR="${INSTALL_DIR}/openssl"
    ./Configure ${HOST_SYSTEM_NAME} no-shared no-asm no-async --cross-compile-prefix=${COMPILER_PREFIX} -I"${TOOLCHAIN_INCLUDE}" -L"${TOOLCHAIN_LIB}" --prefix=${OPENSSL_ROOT_DIR}   -DOPENSSL_THREADS -pthread -D_REENTRANT -D_THREAD_SAFE -D_THREADSAFE --openssldir=${OPENSSL_ROOT_DIR}
    # make CC=${C_COMPILER} LD=${LD} RANLIB=${COMPILER_PREFIX}ranlib MAKEDEPPROG=${C_COMPILER} AR=${AR} PROCESSOR=ARM
    make install_sw
    OPENSSL_INCLUDE_DIR="${OPENSSL_ROOT_DIR}/include"
    OPENSSL_LIB_DIR="${OPENSSL_ROOT_DIR}/lib"
    OPENSSL_CRYPTO_LIBRARY="${OPENSSL_LIB_DIR}/libcrypto.a"
    OPENSSL_SSL_LIBRARY="${OPENSSL_LIB_DIR}/libssl.a"
    cd ${CUR_PATH}
}

build_libevent()
{
    cd ${SRC_PATH}/libevent
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/libevent ${COMPILER} -DEVENT__LIBRARY_TYPE="STATIC" -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR} -DEVENT__DISABLE_MBEDTLS=TRUE -DCMAKE_CXX_FLAGS="-I${OPENSSL_INCLUDE_DIR} -L${OPENSSL_LIB_DIR} -lssl -lcrypto -ldl" ..
    # ./autogen.sh
    # ./configure --host=${CONFIGURE_HOST} --enable-static --disable-shared --prefix=${INSTALL_DIR}/libevent CPPFLAGS="-I${OPENSSL_INCLUDE_DIR}" LDFLAGS="-L${OPENSSL_LIB_DIR} -lssl -lcrypto -ldl" CC=${C_COMPILER} CXX=${CXX_COMPILER}
    make -j$(nproc) & make install
    LIBEVENT_INCLUDE_DIR="${INSTALL_DIR}/libevent/include"
    LIBEVENT_LIB="${INSTALL_DIR}/libevent/lib"
    cd ${CUR_PATH}
}

build_folly()
{
    build_fmt 
    build_openssl
    build_boost
    build_uuid
    build_gtest
    build_double-conversion
    build_gflags
    build_glog
    build_libevent
    
    cd ${SRC_PATH}/folly
    if [ -d _build ]; then
        rm -rf _build
    fi
    mkdir -p _build 
    cd ./_build
    cmake -Dfmt_DIR=${INSTALL_DIR}/fmt -DLIBFMT_INCLUDE_DIR=${LIBFMT_INCLUDE_DIR} -DLIBFMT_LIBRARY=${LIBFMT_LIBRARY_DIR} -DDOUBLE_CONVERSION_INCLUDE_DIR=${DOUBLE_CONVERSION_INCLUDE_DIR} -DDOUBLE_CONVERSION_LIBRARY=${DOUBLE_CONVERSION_LIBRARY_DIR} -DBOOST_ROOT=${INSTALL_DIR}/boost  -DBoost_NO_BOOST_CMAKE=TRUE -DGLOG_INCLUDEDIR=${GLOG_INCLUDE_DIR} -DGLOG_LIBRARYDIR=${GLOG_LIBRARY_DIR} -DLIBEVENT_INCLUDE_DIR=${LIBEVENT_INCLUDE_DIR} -DLIBEVENT_LIB=${LIBEVENT_LIB} -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR} -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/folly ${COMPILER} -DCMAKE_USE_OPENSSL=False -DOPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR} -DOPENSSL_CRYPTO_LIBRARY=${OPENSSL_CRYPTO_LIBRARY} -DOPENSSL_SSL_LIBRARY=${OPENSSL_SSL_LIBRARY} -DBUILD_TESTS=OFF -DCMAKE_CXX_FLAGS="-I${GLOG_INCLUDE_DIR}" -DCOMPILER_HAS_F_ALIGNED_NEW=OFF -DFOLLY_USE_JEMALLOC=0 -DCMAKE_PREFIX_PATH=${INSTALL_DIR}/fmt -DFOLLY_HAVE_MEMRCHR=ON -DFOLLY_HAVE_CLOCK_GETTIME=ON -DUNIX=ON -DCMAKE_C_FLAGS="${CMAKE_C_FLAGS} -Wl,-lrt" ..
    make -j$(nproc) & make install
    cd ${CUR_PATH}
}

# ================ cpprest ================
build_zlib()
{
    cd ${SRC_PATH}/zlib
    ZLIB_ROOT_DIR="${INSTALL_DIR}/zlib"
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build
    cd ./build
    cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=${ZLIB_ROOT_DIR} -DAMD64=OFF -DASM686=OFF ${COMPILER} ..
    make -j$(nproc) & make install
    rm ${ZLIB_ROOT_DIR}/lib/libz.so*
    ZLIB_INCLUDE_DIR="${ZLIB_ROOT_DIR}/include"
    ZLIB_LIBRARY="${ZLIB_ROOT_DIR}/lib/libz.a"
    cd ${CUR_PATH}
}

build_cpprest()
{
    build_zlib

    cd ${SRC_PATH}/cpprest
    if [ -d build ]; then
        rm -rf build
    fi
    sed -i "3s%^%# %g" ./CMakeLists.txt
    mkdir -p build
    cd ./build
    cmake ${COMPILER}                                       \
        -DCMAKE_CXX_COMPILER_FORCED=TRUE    \
        -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/cpprest       \
        -DCMAKE_MODULE_PATH="${BOOST_ROOT}/lib/cmake"       \
        -DBOOST_ROOT=${BOOST_ROOT}                          \
        -DBoost_INCLUDE_DIR=${BOOST_INCLUDE_DIR}            \
        -DBoost_LIBRARIES=${BOOST_LIBRARY_DIR}              \
        -DZLIB_INCLUDE_DIR=${ZLIB_INCLUDE_DIR}              \
        -DZLIB_LIBRARY=${ZLIB_LIBRARY}                      \
        -DOPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}        \
        -DOPENSSL_CRYPTO_LIBRARY=${OPENSSL_CRYPTO_LIBRARY}  \
        -DOPENSSL_SSL_LIBRARY=${OPENSSL_SSL_LIBRARY}        \
        -DWERROR=OFF                                        \
        -DCMAKE_CXX_FLAGS="-L${OPENSSL_LIB_DIR}"            \
        -DBUILD_TESTS=OFF                                   \
        -DBUILD_SAMPLES=OFF                                 \
        -DBoost_USE_STATIC_LIBS=ON                          \
        -DBUILD_SHARED_LIBS=OFF                             \
        ..
    make -j$(nproc) & make install
    cd ${CUR_PATH}
}

# ================ FFMpeg ================
build_ffmpeg()
{
    cd ${SRC_PATH}/ffmpeg
    if [[ ${PLATFORM} == "rk" ]];
    then
        ./configure --prefix=${INSTALL_DIR}/ffmpeg --pkg-config=pkg-config --pkg-config-flags=${INSTALL_DIR}/rk_mpp/lib/pkgconfig/rockchip_mpp.pc --enable-version3 --enable-ffmpeg  --disable-ffplay --disable-ffprobe --disable-doc --disable-htmlpages --disable-podpages --disable-txtpages --cross-prefix=${COMPILER_PREFIX} --target-os=linux --enable-cross-compile --arch=aarch64 --enable-nonfree --enable-gpl # --enable-rkmpp --enable-libdrm
    else
        ./configure --prefix=${INSTALL_DIR}/ffmpeg --pkg-config=pkg-config --enable-version3 --enable-ffmpeg  --disable-ffplay --disable-ffprobe --disable-doc --disable-htmlpages --disable-podpages --disable-txtpages --cross-prefix=${COMPILER_PREFIX} --target-os=linux --enable-cross-compile --arch=arm --enable-nonfree --enable-gpl # --enable-rkmpp --enable-libdrm
    fi
    
    make -j$(nproc)
    make install
    cd ${CUR_PATH}
}

# ================ freetype ================
build_freetype()
{
    cd ${SRC_PATH}/freetype
    cp -rf ./freetype ${INSTALL_DIR}/
    cp -rf ./SDL ${INSTALL_DIR}/
    cd ${CUR_PATH}
}

# ================ hisi_mpp ================
build_hisi_mpp()
{
    cd ${SRC_PATH}/hisi_mpp
    mkdir -p ${INSTALL_DIR}/hisi_mpp   
    cp -rf ./include ${INSTALL_DIR}/hisi_mpp/
    cp -rf ./lib ${INSTALL_DIR}/hisi_mpp/
    cd ${CUR_PATH}
}

# ================ mosquitto ================
build_mosquitto()
{
    cd ${SRC_PATH}/mosquitto
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build 
    cd ./build

    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/mosquitto ${COMPILER} -DDOCUMENTATION=OFF -DWITH_STATIC_LIBRARIES=ON -DWITH_PLUGINS=OFF -DOPENSSL_ROOT_DIR=${OPENSSL_ROOT_DIR} -DOPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR} -DOPENSSL_SSL_LIBRARY=${OPENSSL_SSL_LIBRARY} -DOPENSSL_CRYPTO_LIBRARY=${OPENSSL_CRYPTO_LIBRARY} ..
    make -j$(nproc) & make install
    cd ${CUR_PATH}
}

# ================ OSS ================
build_libcurl()
{    
    cd ${SRC_PATH}/libcurl
    autoreconf -fi
    ./configure --host=${CONFIGURE_HOST} --enable-static --disable-shared --prefix=${INSTALL_DIR}/libcurl PKG_CONFIG_PATH=${OPENSSL_LIB_DIR}/pkgconfig CC=${C_COMPILER} CXX=${CXX_COMPILER} --with-openssl
    make -j$(nproc) & make install

    CURL_ROOT_DIR="${INSTALL_DIR}/libcurl"
    CURL_INCLUDE_DIR="${CURL_ROOT_DIR}/include"
    CURL_LIBRARY="${CURL_ROOT_DIR}/lib/libcurl.a"
    CURL_CONFIG_BIN="${CURL_ROOT_DIR}/bin/curl-config"

    cd ${CUR_PATH}
}

build_apr_host()
{
    cd ${SRC_PATH}/apr_host
    ./configure --host=x86_64-pc-linux --prefix=${SRC_PATH}/apr_host/install
    make -j$(nproc) & make install
    GEN_CHAR_TOOL="${SRC_PATH}/apr_host/tools/gen_test_char"
    
    cd ${CUR_PATH}
}

build_apr()
{
    build_apr_host

    cd ${SRC_PATH}/apr
    if [ -f arm-linux.cache ]; then
        make clean
        rm arm-linux.cache
    fi
    mkdir -p include/private
    ${GEN_CHAR_TOOL} > include/private/apr_escape_test_char.h
    ./configure --host=${CONFIGURE_HOST} --enable-static --disable-shared --prefix=${INSTALL_DIR}/apr CC=${C_COMPILER} CXX=${CXX_COMPILER} ac_cv_file__dev_zero=yes ac_cv_func_setpgrp_void=yes apr_cv_tcp_nodelay_with_cork=yes --cache=arm-linux.cache ac_cv_sizeof_struct_iovec=8 ap_cv_void_ptr_lt_long=no
    sed -i "144s%tools/gen_test_char%${GEN_CHAR_TOOL}%g" Makefile
    sed -i "134,137s%^%# %g" Makefile
    sed -i "139,140s%^%# %g" Makefile
    make all & make install

    APR_ROOT_DIR="${INSTALL_DIR}/apr"
    APR_INCLUDE_DIR="${APR_ROOT_DIR}/include/apr-1"
    APR_LIBRARY="${APR_ROOT_DIR}/lib/libapr-1.a"
    APR_CONFIG_BIN="${APR_ROOT_DIR}/bin/apr-1-config"
    cd ${CUR_PATH}
}

build_apr_util()
{
    cd ${SRC_PATH}/apr-util
    if [ -f arm-linux.cache ]; then
        make clean
        rm arm-linux.cache
    fi
    ./configure --host=${CONFIGURE_HOST} --enable-static --disable-shared --prefix=${INSTALL_DIR}/apr-util CC=${C_COMPILER} CXX=${CXX_COMPILER}ac_cv_file__dev_zero=yes ac_cv_func_setpgrp_void=yes apr_cv_tcp_nodelay_with_cork=yes --cache=arm-linux.cache ac_cv_sizeof_struct_iovec=8 ap_cv_void_ptr_lt_long=no  --with-expat=builtin --with-apr=${APR_ROOT_DIR}
    make -j$(nproc) & make install

    APR_UTIL_ROOT_DIR="${INSTALL_DIR}/apr-util"
    APR_UTIL_INCLUDE_DIR="${APR_UTIL_ROOT_DIR}/include"
    APR_UTIL_LIBRARY="${APR_UTIL_ROOT_DIR}/lib/libaprutil-1.a"
    APU_CONFIG_BIN="${APR_UTIL_ROOT_DIR}/bin/apu-1-config"
    cd ${CUR_PATH}
}

build_mxml()
{
    cd ${SRC_PATH}/mxml
    ./configure --host=${CONFIGURE_HOST} --prefix=${INSTALL_DIR}/mxml CC=${C_COMPILER} CXX=${CXX_COMPILER} STRIP=${COMPILER_PREFIX}strip --enable-shared=no
    sed -i "91s%mxmldoc testmxml mxml.xml doc/mxml.man%%g" Makefile
    sed -i "143,150s%^%# %g" Makefile
    sed -i "157,161s%^%# %g" Makefile
    make -j$(nproc)
    mkdir -p ${INSTALL_DIR}/mxml
    mkdir -p ${INSTALL_DIR}/mxml/include
    cp mxml.h ${INSTALL_DIR}/mxml/include/
    make install

    MXML_ROOT_DIR="${INSTALL_DIR}/mxml"
    MINIXML_INCLUDE_DIR="${MXML_ROOT_DIR}/include"
    MINIXML_LIBRARY="${MXML_ROOT_DIR}/lib/libmxml.a"
    cd ${CUR_PATH}
}

build_oss()
{
    build_libcurl
    build_apr
    build_apr_util
    build_mxml

    cd ${SRC_PATH}/oss_c_sdk
    sed -i "124,125s%^%# %g" ./CMakeLists.txt
    if [ -d build ]; then
        rm -rf build
    fi
    mkdir -p build
    cd ./build

    cmake ${COMPILER}                                       \
        -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/oss_c_sdk     \
        -DCURL_INCLUDE_DIR=${CURL_INCLUDE_DIR}              \
        -DCURL_LIBRARY=${CURL_LIBRARY}                      \
        -DCURL_CONFIG_BIN=${CURL_CONFIG_BIN}                \
        -DAPR_INCLUDE_DIR=${APR_INCLUDE_DIR}                \
        -DAPR_LIBRARY=${APR_LIBRARY}                        \
        -DAPR_UTIL_INCLUDE_DIR=${APR_UTIL_INCLUDE_DIR}      \
        -DAPR_UTIL_LIBRARY=${APR_UTIL_LIBRARY}              \
        -DMINIXML_INCLUDE_DIR=${MINIXML_INCLUDE_DIR}        \
        -DMINIXML_LIBRARY=${MINIXML_LIBRARY}                \
        -DAPR_CONFIG_BIN=${APR_CONFIG_BIN}                  \
        -DAPU_CONFIG_BIN=${APU_CONFIG_BIN}                  \
        ..
    make -j$(nproc) & make install
    cd ${CUR_PATH}
}

# ================ RK MPP ================
build_rk_mpp()
{
    cd ${SRC_PATH}/rk_mpp
    mkdir -p build 
    cd ./build
    if [ -f CMakeCache.txt ]; then
        make clean
        rm CMakeCache.txt
    fi
    cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}/rk_mpp ${COMPILER} -DCMAKE_BUILD_TYPE=Release -DRKPLATFORM=ON -DHAVE_DRM=ON -DSYSPROC=armv7-a -DBUILD_TEST=OFF -DENABLE_STATIC=ON -DCMAKE_BUILD_TYPE=Release ..
    make -j$(nproc) & make install
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

# ================ x264 ================
build_x264()
{
    cd ${SRC_PATH}/x264
    ./configure --host=${CONFIGURE_HOST} --prefix=${INSTALL_DIR}/x264 --cross-prefix=${COMPILER_PREFIX} --disable-asm --enable-static --disable-opencl
    make -j$(nproc) & make install-lib-static
    cd ${CUR_PATH}
}

build_hisi_runtime()
{
    cd ${SRC_PATH}/hisi_runtime
    mkdir -p ${INSTALL_DIR}/hisi_runtime/
    cp -rP ./lib ${INSTALL_DIR}/hisi_runtime
    cd ${CUR_PATH}
}

# 构建：海思
build_for_hisi()
{
    echo "================ Build for hisi ================"
    INSTALL_DIR="${CUR_PATH}/${LIB_PATH}/${HISI_LIB_PATH}"
    COMPILER_PREFIX="arm-himix200-linux-"
    C_COMPILER="${COMPILER_PREFIX}gcc"
    CXX_COMPILER="${COMPILER_PREFIX}g++"
    LD="${COMPILER_PREFIX}ld"
    AR="${COMPILER_PREFIX}ar"
    COMPILER="-DCMAKE_C_COMPILER=${C_COMPILER} -DCMAKE_CXX_COMPILER=${CXX_COMPILER}"
    BUILD_PLATFORM="hisi"
    HOST_SYSTEM_NAME="linux-generic32"
    CONFIGURE_HOST="arm-none-linux"
    TOOLCHAIN_ROOT="$(dirname `which ${C_COMPILER}`)/../arm-linux-gnueabi"
    TOOLCHAIN_INCLUDE="${TOOLCHAIN_ROOT}/include"
    TOOLCHAIN_LIB="${TOOLCHAIN_ROOT}/lib"

    export LD_LIBRARY_PATH=

    build_opencv
    build_folly
    build_cpprest
    build_freetype
    build_hisi_mpp
    build_oss
    build_x264
    build_hisi_runtime

    echo "================ Build for hisi DONE ================"
}

# 构建: RK
build_for_rk()
{
    echo "================ Build for RK ================"
    INSTALL_DIR="${CUR_PATH}/${LIB_PATH}/${RK_LIB_PATH}"
    COMPILER_PREFIX="aarch64-linux-gnu-"
    C_COMPILER="${COMPILER_PREFIX}gcc"
    CXX_COMPILER="${COMPILER_PREFIX}c++"
    LD="${COMPILER_PREFIX}ld"
    AR="${COMPILER_PREFIX}ar"
    RANLIB="${COMPILER_PREFIX}ranlib"
    COMPILER="-DCMAKE_C_COMPILER=${C_COMPILER} -DCMAKE_CXX_COMPILER=${CXX_COMPILER}"
    BUILD_PLATFORM="rk"
    HOST_SYSTEM_NAME="linux-aarch64"
    CONFIGURE_HOST="aarch64-none-linux"
    TOOLCHAIN_ROOT="$(dirname `which ${C_COMPILER}`)/../"
    TOOLCHAIN_INCLUDE="${TOOLCHAIN_ROOT}/include"
    TOOLCHAIN_LIB="${TOOLCHAIN_ROOT}/lib"

    export LD_LIBRARY_PATH=
    
    build_fmt
    build_opencv
    build_folly
    build_cpprest
    build_rk_mpp
    build_ffmpeg
    build_tengine
    build_oss
    build_x264

    echo "================ Build for RK DONE ================"
}

# 构建
build_all()
{
    if [[ ${PLATFORM} == "all" ]] || [[ ${PLATFORM} == "hisi" ]];
    then
        build_for_hisi
    fi

    if [[ ${PLATFORM} == "all" ]] || [[ ${PLATFORM} == "rk" ]];
    then
        build_for_rk
    fi

    
}

setup
download_all
# build_for_hisi

if [ -f "algo_dependencies.tar.gz" ]; 
then
    rm algo_dependencies.tar.gz
fi
tar zcf algo_dependencies.tar.gz ${LIB_PATH}
echo "================ Tar to algo_dependencies.tar.gz ================"