package com.cxp.learningvideo

import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_ffmpeg_info.*


/**
 * FFmpeg测试页面
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 *
 */
class FFmpegActivity: AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ffmpeg_info)
        tv.text = ffmpegInfo()
    }

    private external fun ffmpegInfo(): String

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}