package com.cxp.learningvideo.opengl.drawer

import android.opengl.GLES11Ext
import android.opengl.GLES20
import android.opengl.Matrix
import android.util.Log
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.nio.FloatBuffer


/**
 * 基础绘制类
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-19-09 08:55
 *
 */
abstract class ADrawer(protected val mTextureId: Int,
                       protected val mWorldWidth: Int,
                       protected val mWorldHeight: Int,
                       protected val mOriginWidth: Int,
                       protected val mOriginHeight: Int) {

    private val TAG = "ADrawer"

    /**上下颠倒的顶点矩阵*/
    private val mReserveVertexCoors = floatArrayOf(
        -1f, 1f,
        1f, 1f,
        -1f, -1f,
        1f, -1f
    )

    private val mVertexCoors = floatArrayOf(
        -1f, -1f,
        1f, -1f,
        -1f, 1f,
        1f, 1f
    )

    private val mTextureCoors = floatArrayOf(
        0f, 1f,
        1f, 1f,
        0f, 0f,
        1f, 0f
    )

    private var mProgram: Int = -1

    private var mVertexMatrixHandler: Int = -1
    private var mVertexPosHandler: Int = -1
    private var mTexturePosHandler: Int = -1
    private var mTextureHandler: Int = -1

    private lateinit var mVertexBuffer: FloatBuffer
    private lateinit var mTextureBuffer: FloatBuffer

    private var mPrjMatrix = FloatArray(16)
    private val mViewMatrix = FloatArray(16)
    protected val mDefMatrix = FloatArray(16)

    private var mWActualNormalRatio = 1f
    private var mHActualNormalRatio = 1f

    private var mCurX = 0
    private var mCurY = 0

    init {
        initPos()
        initDefMatrix()
    }

    private fun initPos() {
        val bb = ByteBuffer.allocateDirect(mVertexCoors.size * 4)
        bb.order(ByteOrder.nativeOrder())
        //将坐标数据转换为FloatBuffer，用以传入给OpenGL ES程序
        mVertexBuffer = bb.asFloatBuffer()
        mVertexBuffer.put(mVertexCoors)
        mVertexBuffer.position(0)

        val cc = ByteBuffer.allocateDirect(mTextureCoors.size * 4)
        cc.order(ByteOrder.nativeOrder())
        mTextureBuffer = cc.asFloatBuffer()
        mTextureBuffer.put(mTextureCoors)
        mTextureBuffer.position(0)
    }

    private fun initDefMatrix() {
        val originRatio = mOriginWidth/ mOriginHeight.toFloat()
        val worldRatio = mWorldWidth / mWorldHeight.toFloat()
        if (mWorldWidth > mWorldHeight) {
            if (originRatio > worldRatio) {
                mWActualNormalRatio = worldRatio * originRatio
                Matrix.orthoM(mPrjMatrix, 0,
                    -mWActualNormalRatio,
                    mWActualNormalRatio,
                    -1f, 1f, 3f, 5f)
                mCurX = ((mWorldWidth / 2 - mWorldWidth / mWActualNormalRatio).toInt())
            } else {// 原始比例小于窗口比例，缩放高宽度会导致宽度度超出，因此，宽度度以窗口为准，缩放高度
                mHActualNormalRatio = worldRatio * originRatio
                Matrix.orthoM(mPrjMatrix, 0,
                    -1f, 1f,
                    -mWActualNormalRatio,
                    mWActualNormalRatio, 3f, 5f)
                mCurY = (mWorldHeight / 2 - mWorldHeight / 2 / mHActualNormalRatio).toInt()
            }

        } else {
            if (originRatio > worldRatio) {
                mHActualNormalRatio = originRatio / worldRatio
                Matrix.orthoM(mPrjMatrix, 0,
                    -1f, 1f,
                    -mHActualNormalRatio,
                    mHActualNormalRatio,
                    3f, 5f)
                mCurY = (mWorldHeight / 2 - mWorldHeight / 2 / mHActualNormalRatio).toInt()
            } else {// 原始比例小于窗口比例，缩放高度会导致高度超出，因此，高度以窗口为准，缩放宽度
                mWActualNormalRatio = originRatio / worldRatio
                Matrix.orthoM(mPrjMatrix, 0,
                    -mHActualNormalRatio,
                    mHActualNormalRatio,
                    -1f, 1f,
                    3f, 5f)
                mCurX = ((mWorldWidth / 2 - mWorldWidth / mWActualNormalRatio).toInt())
            }
        }
        //设置相机位置
        Matrix.setLookAtM(mViewMatrix, 0,
            0f, 0f, 5.0f,
            0f, 0f, 0f,
            0f, 1.0f, 0.0f)
        //计算变换矩阵
        Matrix.multiplyMM(mDefMatrix, 0, mPrjMatrix, 0, mViewMatrix, 0)
    }

    private fun createGLPrg() {
        if (mProgram == -1) {
            val vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, getVertexShader())
            val fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, getFragmentShader())

            //创建一个空的OpenGLES程序，注意：需要在OpenGL渲染线程中创建，否则无法渲染
            mProgram = GLES20.glCreateProgram()
            //将顶点着色器加入到程序
            GLES20.glAttachShader(mProgram, vertexShader)
            //将片元着色器加入到程序中
            GLES20.glAttachShader(mProgram, fragmentShader)
            //连接到着色器程序
            GLES20.glLinkProgram(mProgram)

            mVertexMatrixHandler = GLES20.glGetUniformLocation(mProgram, "uMatrix")
            mVertexPosHandler = GLES20.glGetAttribLocation(mProgram, "aPosition")
            mTextureHandler = GLES20.glGetUniformLocation(mProgram, "uTexture")
            mTexturePosHandler = GLES20.glGetAttribLocation(mProgram, "aCoordinate")

            initCstShaderHandler()
        }
        //使用OpenGL程序
        GLES20.glUseProgram(mProgram)
    }

    private fun loadShader(type: Int, shaderCode: String): Int {
        //根据type创建顶点着色器或者片元着色器
        val shader = GLES20.glCreateShader(type)
        //将资源加入到着色器中，并编译
        GLES20.glShaderSource(shader, shaderCode)
        GLES20.glCompileShader(shader)

        Log.v(TAG, "Results of compiling source:" + "\n" + shaderCode
                    + "\n:" + GLES20.glGetShaderInfoLog(shader))
        return shader
    }

    fun draw() {
        if (mTextureId != -1) {
            createGLPrg()
            bindTexture()
            prepareDraw()
            doDraw()
            doneDraw()
        } else {
            Log.w(TAG, "Texture id is invalid, can not draw texture")
        }
    }

    private fun doDraw() {
        //启用顶点的句柄
        GLES20.glEnableVertexAttribArray(mVertexPosHandler)
        GLES20.glEnableVertexAttribArray(mTexturePosHandler)
        //设置着色器参数
        GLES20.glUniformMatrix4fv(mVertexMatrixHandler, 1, false, mDefMatrix, 0)
        GLES20.glVertexAttribPointer(mVertexPosHandler, 2, GLES20.GL_FLOAT, false, 0, mVertexBuffer)
        GLES20.glVertexAttribPointer(mTexturePosHandler, 2, GLES20.GL_FLOAT, false, 0, mTextureBuffer)
        //开始绘制
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4)
    }

    protected fun activateTexture(texture: Int = mTextureId,
                                  index: Int = 0,
                                  textureHandler: Int = mTextureHandler) {
        activateTexture(GLES20.GL_TEXTURE_2D, texture, index, textureHandler)
    }

    protected fun activateOESTexture(texture: Int = mTextureId,
                                     index: Int = 0,
                                     textureHandler: Int = mTextureHandler) {
        activateTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texture, index, textureHandler)
    }

    private fun activateTexture(type: Int, texture: Int, index: Int, textureHandler: Int) {
        //激活指定纹理单元
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0 + index)
        //绑定纹理ID到纹理单元
        GLES20.glBindTexture(type, texture)
        //将活动的纹理单元传递到着色器里面
        GLES20.glUniform1i(textureHandler, index)
        //配置边缘过渡参数
        GLES20.glTexParameterf(type, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR.toFloat())
        GLES20.glTexParameterf(type, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR.toFloat())
        GLES20.glTexParameteri(type, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE)
        GLES20.glTexParameteri(type, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE)
    }

    fun translate(x: Int, y: Int) {
        val tx = if (x != mCurX) {
            ((mWorldWidth / 2 - mWorldWidth / 2 / mWActualNormalRatio) - x) / (mWorldWidth / 2 / mWActualNormalRatio)
        } else {
            0f
        }
        val ty = if (y != mCurY) {
            ((mWorldHeight/2 - mWorldHeight / 2 / mHActualNormalRatio) - y) / (mWorldHeight / 2 / mHActualNormalRatio)
        } else {
            0f
        }
        Matrix.translateM(mDefMatrix, 0, tx, ty, 0f)
        mCurX = x
        mCurY = y
    }

    fun scale(sx: Float, sy: Float) {
        Matrix.scaleM(mDefMatrix, 0, sx, sy, 1f)
    }

    fun release() {
        GLES20.glDisableVertexAttribArray(mVertexPosHandler)
        GLES20.glDisableVertexAttribArray(mTexturePosHandler)
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, 0)
        GLES20.glDeleteTextures(1, intArrayOf(mTextureId), 0)
        GLES20.glDeleteProgram(mProgram)
    }

    protected abstract fun getVertexShader(): String
    protected abstract fun getFragmentShader(): String
    protected abstract fun initCstShaderHandler()
    protected abstract fun prepareDraw()
    protected abstract fun bindTexture()
    protected abstract fun doneDraw()
}