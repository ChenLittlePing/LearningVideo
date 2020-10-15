package com.cxp.learningvideo

import android.os.Bundle
import android.os.Environment
import android.support.v7.app.AppCompatActivity
import android.view.View
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_ff_repack.*
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
class FFEncodeActivity: AppCompatActivity() {

    private var ffEncoder: Int = -1

    private val srcPath = Environment.getExternalStorageDirectory().absolutePath + "/mvtest2.mp4"
    private val destPath = Environment.getExternalStorageDirectory().absolutePath + "/mvtest_en_out.mp4"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ff_repack)
        btn.text = "开始编码"

        ffEncoder = initEncoder(srcPath, destPath)
    }

    fun onStartClick(view: View) {
        if (ffEncoder != 0) {
            startEncoder(ffEncoder)
            Toast.makeText(this, "开始编码", Toast.LENGTH_SHORT).show()
        }
    }

    override fun onDestroy() {
        if (ffEncoder > 0) {
            releaseEncoder(ffEncoder)
        }
        super.onDestroy()
    }

    private external fun initEncoder(srcPath: String, destPath: String): Int

    private external fun startEncoder(encoder: Int)

    private external fun releaseEncoder(encoder: Int)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}