cmake_minimum_required(VERSION 3.18)
project(aubio_native LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------
# aubio 源文件
# -------------------------------------------------
set(AUBIO_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/fvec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/cvec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/lvec.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/fmat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/mathutils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/vecutils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/musicutils.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral/fft.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral/phasevoc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral/specdesc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral/awhitening.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral/ooura_fft8g.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral/statistics.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/temporal/filter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/temporal/a_weighting.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/temporal/c_weighting.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/temporal/biquad.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/onset/onset.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/onset/peakpicker.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/tempo/beattracking.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/tempo/tempo.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitch.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchyin.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchfcomb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchmcomb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchschmitt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchspecacf.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchyinfast.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch/pitchyinfft.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/utils/log.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/utils/hist.c
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/utils/scale.c
)

# -------------------------------------------------
# aubio 静态库
# -------------------------------------------------
add_library(aubio STATIC ${AUBIO_SRC})

target_include_directories(aubio PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/include
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/spectral
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/temporal
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/onset
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/tempo
        ${CMAKE_CURRENT_SOURCE_DIR}/aubio/src/pitch
)

target_compile_options(aubio PRIVATE -O3 -ffast-math -fPIC)

# -------------------------------------------------
# JNI 动态库
# -------------------------------------------------
add_library(aubio_jni SHARED native-lib.cpp)

target_link_libraries(aubio_jni
        PRIVATE
        aubio
        log
        android
        m
        c++_static   # ⚡ 静态 STL
)

# arm64-v8a 页大小优化（可选）
if(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
    set_target_properties(aubio_jni PROPERTIES
            LINK_FLAGS "-Wl,-z,max-page-size=0x4000"
    )
endif()
