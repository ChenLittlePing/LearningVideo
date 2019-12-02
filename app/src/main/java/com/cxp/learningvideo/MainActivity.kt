package com.cxp.learningvideo

import android.content.Intent
import android.os.Bundle
import android.support.v7.app.AppCompatActivity
import android.view.View

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
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
}
