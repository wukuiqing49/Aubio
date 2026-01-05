package com.wkq.aubio

/**
 *
 *@Author: wkq
 *
 *@Time: 2026/1/5 16:40
 *
 *@Desc:
 */


import android.util.Log

/**
 * Aubio 音频分析工具类 (Kotlin)
 * 支持 RMS、谱质心、Pitch、Onset、BPM 分析
 */
class AudioAnalyzer {

    init {
        System.loadLibrary("aubio_jni") // 确保 C++ 已编译成 so
    }

    /**
     * Java 回调接口
     */
    interface FeatureCallback {
        fun onFeatureFrame(
            timeSec: Float,
            rms: Float,
            centroid: Float,
            pitch: Float,
            onset: Float,
            bpm: Float
        )

        fun onAnalysisComplete()
    }

    /**
     * 核心分析方法
     *
     * @param pcmData PCM 浮点数组 (-1~1)
     * @param sampleRate 采样率
     * @param winSize 分析窗口，传 0 自动选择
     * @param hopSize 步长，传 0 自动选择
     * @param callback 回调
     */
    fun analyzeFrames(
        pcmData: FloatArray,
        sampleRate: Int,
        winSize: Int = 0,
        hopSize: Int = 0,
        callback: FeatureCallback
    ) {
        if (pcmData.isEmpty()) {
            Log.e("AudioAnalyzer", "PCM 数据为空")
            return
        }
        nativeAnalyzeFrames(pcmData, sampleRate, winSize, hopSize, callback)
    }

    /**
     * JNI 方法，内部调用 C++ Aubio 分析
     */
    private external fun nativeAnalyzeFrames(
        pcmArray: FloatArray,
        sampleRate: Int,
        winSize: Int,
        hopSize: Int,
        callback: FeatureCallback
    )


//    val analyzer = AudioAnalyzer()
//    val pcmData: FloatArray = ... // PCM 浮点数组
//    val sampleRate = 44100
//
//// 自动选择 winSize / hopSize
//    analyzer.analyzeFrames(pcmData, sampleRate, 0, 0, object : AudioAnalyzer.FeatureCallback {
//        override fun onFeatureFrame(timeSec: Float, rms: Float, centroid: Float, pitch: Float, onset: Float, bpm: Float) {
//            Log.d("Analyzer", "time=$timeSec rms=$rms pitch=$pitch onset=$onset")
//        }
//
//        override fun onAnalysisComplete() {
//            Log.d("Analyzer", "分析完成")
//        }
//    })
//
//// 或者手动设置
//    analyzer.analyzeFrames(pcmData, sampleRate, winSize = 2048, hopSize = 512, callback = object : AudioAnalyzer.FeatureCallback {
//        override fun onFeatureFrame(timeSec: Float, rms: Float, centroid: Float, pitch: Float, onset: Float, bpm: Float) {}
//        override fun onAnalysisComplete() {}
//    })

}
