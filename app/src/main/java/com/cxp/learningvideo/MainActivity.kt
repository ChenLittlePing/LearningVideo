package com.cxp.learningvideo

import android.os.Bundle
import android.os.Environment
import android.support.v7.app.AppCompatActivity
import android.view.View
import com.cxp.learningvideo.media.decoder.AudioDecoder
import com.cxp.learningvideo.media.decoder.VideoDecoder
import com.cxp.learningvideo.media.muxer.MP4Repack
import kotlinx.android.synthetic.main.activity_main.*
import java.util.concurrent.Executors

class MainActivity : AppCompatActivity() {
    val path = Environment.getExternalStorageDirectory().absolutePath + "/mvtest_2.mp4"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        initPlayer()
    }

    private fun initPlayer() {
        val videoDecoder = VideoDecoder(path, sfv, null)
        val threadPool = Executors.newFixedThreadPool(10)
        threadPool.execute(videoDecoder)

        val audioDecoder = AudioDecoder(path)
        threadPool.execute(audioDecoder)

        videoDecoder.goOn()
        audioDecoder.goOn()
    }

    fun clickRepack(view: View) {
        repack()
    }

    private fun repack() {
        val repack = MP4Repack(path)
        repack.start()
    }
}
