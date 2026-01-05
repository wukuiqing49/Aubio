#include <algorithm>  // 加上这一行
#include <jni.h>
#include <cmath>
#include <cstring>
#include "aubio.h"
// -------------------------------------------------
// RMS 计算（安全版）
// -------------------------------------------------
static inline float computeRMS(const float* data, uint_t len) {
    float sum = 0.f;
    for (uint_t i = 0; i < len; ++i) sum += data[i] * data[i];
    float rms = sqrtf(sum / len);
    return std::isfinite(rms) ? rms : 0.f;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wkq_aubio_AudioAnalyzer_nativeAnalyzeFrames(
        JNIEnv *env,
        jobject /*thiz*/,
        jfloatArray pcmArray,
        jint sampleRate,
        jint winSize,
        jint hopSize,
        jobject callbackObj) {

    if (!pcmArray || !callbackObj || sampleRate <= 0) return;

    jsize len = env->GetArrayLength(pcmArray);
    if (len <= 0) return;

    // 参数兜底
    if (winSize <= 0) winSize = 1024;
    if (hopSize <= 0) hopSize = winSize / 2;
    if (hopSize < 256) hopSize = 256;

    jfloat* pcmData = env->GetFloatArrayElements(pcmArray, nullptr);
    if (!pcmData) return;

    // -------------------------------------------------
    // Aubio 对象初始化
    // -------------------------------------------------
    fvec_t* in        = new_fvec(winSize);       // 注意：in 长度用 winSize
    fvec_t* pitchOut  = new_fvec(1);
    fvec_t* onsetOut  = new_fvec(1);
    fvec_t* tempoOut  = new_fvec(1);
    cvec_t* spectrum  = new_cvec(winSize);

    aubio_pitch_t* pitchObj = new_aubio_pitch("yin", winSize, hopSize, sampleRate);
    aubio_onset_t* onsetObj = new_aubio_onset("default", winSize, hopSize, sampleRate);
    aubio_tempo_t* tempoObj = new_aubio_tempo("default", winSize, hopSize, sampleRate);
    aubio_fft_t*   fftObj   = new_aubio_fft(winSize);

    auto cleanup = [&]() {
        if (in)        del_fvec(in);
        if (pitchOut)  del_fvec(pitchOut);
        if (onsetOut)  del_fvec(onsetOut);
        if (tempoOut)  del_fvec(tempoOut);
        if (spectrum)  del_cvec(spectrum);
        if (pitchObj)  del_aubio_pitch(pitchObj);
        if (onsetObj)  del_aubio_onset(onsetObj);
        if (tempoObj)  del_aubio_tempo(tempoObj);
        if (fftObj)    del_aubio_fft(fftObj);
        if (pcmData)   env->ReleaseFloatArrayElements(pcmArray, pcmData, JNI_ABORT);
    };

    if (!in || !pitchOut || !onsetOut || !tempoOut ||
        !spectrum || !pitchObj || !onsetObj || !tempoObj || !fftObj) {
        cleanup();
        return;
    }

    // Java 回调
    jclass callbackClass = env->GetObjectClass(callbackObj);
    if (!callbackClass) { cleanup(); return; }

    jmethodID onFeatureFrame = env->GetMethodID(
            callbackClass,
            "onFeatureFrame",
            "(FFFFFF)V"
    );

    jmethodID onComplete = env->GetMethodID(
            callbackClass,
            "onAnalysisComplete",
            "()V"
    );

    if (!onFeatureFrame || !onComplete) { cleanup(); return; }

    // -------------------------------------------------
    // 主分析循环
    // -------------------------------------------------
    for (int i = 0; i < len; i += hopSize) {

        int copyLen = std::min(hopSize, len - i);

        // 清零整个窗口，保证 Aubio pitch 不出异常值
        memset(in->data, 0, sizeof(float) * winSize);
        memcpy(in->data, pcmData + i, sizeof(float) * copyLen);

        float timeSec = (float)i / (float)sampleRate;

        // pitch
        aubio_pitch_do(pitchObj, in, pitchOut);
        float pitchHz = pitchOut->data[0];

        // onset
        aubio_onset_do(onsetObj, in, onsetOut);
        float onsetValue = onsetOut->data[0];

        // tempo
        aubio_tempo_do(tempoObj, in, tempoOut);
        float bpm = tempoOut->data[0];

        // FFT → spectral centroid
        aubio_fft_do(fftObj, in, spectrum);
        float centroid = 0.f;
        float sum = 0.f;
        for (uint_t k = 0; k < spectrum->length; ++k) {
            centroid += k * spectrum->norm[k];
            sum += spectrum->norm[k];
        }
        centroid = (sum > 0.f) ? (centroid / sum) * ((float)sampleRate / winSize) : 0.f;

        // RMS
        float rms = computeRMS(in->data, hopSize);

        env->CallVoidMethod(
                callbackObj,
                onFeatureFrame,
                timeSec,
                rms,
                centroid,
                pitchHz,
                onsetValue,
                bpm
        );

        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            break;
        }
    }

    env->CallVoidMethod(callbackObj, onComplete);

    cleanup();
}
