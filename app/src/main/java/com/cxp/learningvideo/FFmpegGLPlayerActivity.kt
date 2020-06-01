package com.cxp.learningvideo

import android.os.Bundle
import android.os.Environment
import android.support.v7.app.AppCompatActivity
import android.view.Surface
import android.view.SurfaceHolder
import android.widget.Toast
import kotlinx.android.synthetic.main.activity_ff_gl_player.*
import java.io.File


/**
 * FFmpeg + OpenGL 播放器
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2020-06-01 09:04
 *
 */
class FFmpegGLPlayerActivity: AppCompatActivity() {

    val path = Environment.getExternalStorageDirectory().absolutePath + "/mvtest.mp4"

    private var player: Int? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_ff_gl_player)
        initSfv()
    }

    private fun initSfv() {
        if (File(path).exists()) {
            sfv.holder.addCallback(object : SurfaceHolder.Callback {
                override fun surfaceChanged(holder: SurfaceHolder, format: Int,
                                            width: Int, height: Int) {}
                override fun surfaceDestroyed(holder: SurfaceHolder) {
                    stop(player!!)
                }

                override fun surfaceCreated(holder: SurfaceHolder) {
                    if (player == null) {
                        player = createGLPlayer(path, holder.surface)
                        playOrPause(player!!)
                    }
                }
            })
        } else {
            Toast.makeText(this, "视频文件不存在，请在手机根目录下放置 mvtest.mp4", Toast.LENGTH_SHORT).show()
        }
    }

    private external fun createGLPlayer(path: String, surface: Surface): Int
    private external fun playOrPause(player: Int)
    private external fun stop(player: Int)

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }
}