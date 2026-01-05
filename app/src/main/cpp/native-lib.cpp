#include <jni.h>
#include <vector>
#include <cmath>
#include <cstring>
#include "aubio.h"

// 计算 RMS
inline float computeRMS(float* data, int start, int end) {
    float sum = 0;
    for (int i = start; i < end; ++i) sum += data[i] * data[i];
    return sqrtf(sum / (end - start));
}

// 自动选择 winSize
inline uint_t autoWinSize(jsize pcmLen) {
    if (pcmLen < 8000) return 512;
    if (pcmLen < 20000) return 1024;
    return 2048;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wkq_aubio_AudioAnalyzer_analyzeFrames(
        JNIEnv *env,
        jobject thiz,
        jfloatArray pcmArray,
        jint sampleRate,
        jint winSize,     // 分析窗口，可传 0 自动选择
        jint hopSize,     // 步长，可传 0 自动选择
        jobject callbackObj) {

    jsize len = env->GetArrayLength(pcmArray);
    jfloat* pcmData = env->GetFloatArrayElements(pcmArray, nullptr);

    // 自动设置 winSize / hopSize
    if (winSize <= 0) winSize = autoWinSize(len);
    if (hopSize <= 0) hopSize = winSize / 2;

    // 初始化 Aubio 对象
    fvec_t* in = new_fvec(winSize);
    fvec_t* pitchOut = new_fvec(1);
    cvec_t* spectrum = new_cvec(winSize);

    aubio_fft_t* fftObj = new_aubio_fft(winSize);
    aubio_pitch_t* pitchObj = new_aubio_pitch("yin", winSize, hopSize, sampleRate);
    aubio_onset_t* onsetObj = new_aubio_onset("default", winSize, hopSize, sampleRate);
    aubio_tempo_t* tempoObj = new_aubio_tempo("default", winSize, hopSize, sampleRate);

    // 获取 Java 回调方法
    jclass callbackClass = env->GetObjectClass(callbackObj);
    jmethodID onFeatureFrame = env->GetMethodID(callbackClass, "onFeatureFrame",
                                               "(FFFFFF)V");
    jmethodID onComplete = env->GetMethodID(callbackClass, "onAnalysisComplete", "()V");

    // 核心循环
    float lastOnsetTime = -1.0f;

    for (int i = 0; i + winSize <= len; i += hopSize) {
        std::memcpy(in->data, pcmData + i, sizeof(float) * winSize);

        float timeSec = i / (float)sampleRate;

        // pitch
        aubio_pitch_do(pitchObj, in, pitchOut);
        float pitchHz = pitchOut->data[0];

        // onset，用 get_last 判断是否触发
        aubio_onset_do(onsetObj, in, nullptr);
        float onsetLastTime = aubio_onset_get_last(onsetObj);
        float onsetValue = (onsetLastTime > lastOnsetTime) ? 1.0f : 0.0f;
        lastOnsetTime = onsetLastTime;

        // tempo
        aubio_tempo_do(tempoObj, in, nullptr);
        float bpm = aubio_tempo_get_bpm(tempoObj);

        // FFT → spectral centroid (Hz)
        aubio_fft_do(fftObj, in, spectrum);
        float centroid = 0, sum = 0;
        for (uint_t k = 0; k < spectrum->length; k++) {
            centroid += k * spectrum->norm[k];
            sum += spectrum->norm[k];
        }
        centroid = sum > 0 ? (centroid / sum) * (sampleRate / winSize) : 0;

        // RMS
        float rms = computeRMS(in->data, 0, winSize);

        // 调用 Java 回调
        env->CallVoidMethod(callbackObj, onFeatureFrame,
                            timeSec, rms, centroid, pitchHz, onsetValue, bpm);

        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            break;
        }
    }

    // 分析完成回调
    env->CallVoidMethod(callbackObj, onComplete);

    // 释放资源
    del_fvec(in);
    del_fvec(pitchOut);
    del_cvec(spectrum);
    del_aubio_fft(fftObj);
    del_aubio_pitch(pitchObj);
    del_aubio_onset(onsetObj);
    del_aubio_tempo(tempoObj);

    env->ReleaseFloatArrayElements(pcmArray, pcmData, JNI_ABORT);
}
