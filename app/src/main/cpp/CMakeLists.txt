cmake_minimum_required(VERSION 3.18)
project(aubio_native LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 编译优化
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -ffast-math")

# --------------------------
# aubio 源文件列表
# --------------------------
set(AUBIO_SRC
        # 基础核心
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/fvec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/cvec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/lvec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/fmat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/mathutils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/vecutils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/musicutils.c

        # spectral
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral/fft.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral/phasevoc.c

        # onset
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/onset/onset.c



        # tempo
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/tempo/tempo.c

        # pitch
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitch.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchyin.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchfcomb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchmcomb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchschmitt.c

        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchspecacf.c


        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/utils/log.c
)


# --------------------------
# 创建静态库 aubio
# --------------------------
add_library(aubio STATIC ${AUBIO_SRC})

target_include_directories(aubio PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/include
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/onset
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/tempo
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch
)

# --------------------------
# 创建 JNI 动态库
# --------------------------
add_library(native-lib SHARED native-lib.cpp)

# 链接 aubio + 系统库
target_link_libraries(native-lib PRIVATE aubio log android)
