package com.cxp.learningvideo

import android.content.Intent
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.View
import android.widget.Toast
import com.yanzhenjie.permission.AndPermission
import com.yanzhenjie.permission.Permission

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        requestPermission()
    }

    private fun requestPermission() {
        val permissions = Permission.Group.STORAGE
        AndPermission.with(this)
            .runtime()
            .permission(permissions)
            .onGranted {
            }
            .onDenied {
                Toast.makeText(this, "请打开权限，否则无法获取本地文件", Toast.LENGTH_SHORT).show()
            }
            .start()
    }

    fun clickSimplePlayer(view: View) {
        startActivity(Intent(this, SimplePlayerActivity::class.java))
    }

    fun clickSimpleTriangle(view: View) {
        val intent = Intent(this, SimpleRenderActivity::class.java)
        intent.putExtra("type", 0)
        startActivity(intent)
    }

    fun clickSimpleTexture(view: View) {
        val intent = Intent(this, SimpleRenderActivity::class.java)
        intent.putExtra("type", 1)
        startActivity(intent)
    }

    fun clickOpenGLPlayer(view: View?) {
        startActivity(Intent(this, OpenGLPlayerActivity::class.java))
    }

    fun clickMultiOpenGLPlayer(view: View?) {
        startActivity(Intent(this, MultiOpenGLPlayerActivity::class.java))
    }

    fun clickEGLPlayer(view: View?) {
        startActivity(Intent(this, EGLPlayerActivity::class.java))
    }

    fun clickSoulPlayer(view: View?) {
        startActivity(Intent(this, SoulPlayerActivity::class.java))
    }

    fun clickEncoder(view: View?) {
        startActivity(Intent(this, SynthesizerActivity::class.java))
    }

    fun clickFFmpegInfo(view: View?) {
        startActivity(Intent(this, FFmpegActivity::class.java))
    }

    fun clickFFmpegGLPlayer(view: View?) {
        startActivity(Intent(this, FFmpegGLPlayerActivity::class.java))
    }

    fun clickFFmpegRepack(view: View?) {
        startActivity(Intent(this, FFRepackActivity::class.java))
    }

    fun clickFFmpegEncode(view: View?) {
        startActivity(Intent(this, FFEncodeActivity::class.java))
    }
}
