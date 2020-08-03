package com.cxp.learningvideo

import android.os.Bundle
import android.os.Environment
import android.support.v7.app.AppCompatActivity
import android.view.View
import kotlin.concurrent.thread


/**
 * FFmpeg 音视频重打包
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2020-08-02 14:27
 *
 */
class FFRepackActivity: AppCompatActivity() {

    private var ffRepack: Int = 0

    private val srcPath = Environment.getExternalStorageDirectory().absolutePath + "/mvtest.mp4"
    private val destPath = Environment.getExternalStorageDirectory().absolutePath + "/mvtest_repack.mp4"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ff_repack)

        ffRepack = createRepack(srcPath, destPath)
    }

    fun onStartClick(view: View) {
        if (ffRepack != 0) {
            thread {
                startRepack(ffRepack)
            }
        }
    }

    private external fun createRepack(srcPath: String, destPath: String): Int

    private external fun startRepack(repack: Int)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}