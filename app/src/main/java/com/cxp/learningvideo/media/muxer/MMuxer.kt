package com.cxp.learningvideo.media.muxer

import android.media.MediaCodec
import android.media.MediaFormat
import android.media.MediaMuxer
import android.os.Environment
import android.util.Log
import java.nio.ByteBuffer


/**
 * 音视频封装器
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-09-19 11:02
 *
 */
class MMuxer {

    private val TAG = "MMuxer"

    private var mPath: String

    private var mMediaMuxer: MediaMuxer? = null

    private var mVideoTrackIndex = -1
    private var mAudioTrackIndex = -1

    private var mIsAudioTrackAdd = false
    private var mIsVideoTrackAdd = false

    private var mIsAudioEnd = false
    private var mIsVideoEnd = false

    private var mIsStart = false

    private var mStateListener: IMuxerStateListener?  = null

    init {
        val fileName = "LVideo_Test" + /*SimpleDateFormat("yyyyMM_dd-HHmmss").format(Date()) +*/ ".mp4"
        val filePath = Environment.getExternalStorageDirectory().absolutePath.toString() + "/"
        mPath = filePath + fileName
        mMediaMuxer = MediaMuxer(mPath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
    }

    fun addVideoTrack(mediaFormat: MediaFormat) {
        if (mIsVideoTrackAdd) return
        if (mMediaMuxer != null) {
            mVideoTrackIndex = try {
                mMediaMuxer!!.addTrack(mediaFormat)
            } catch (e: Exception) {
                e.printStackTrace()
                return
            }

            Log.i(TAG, "添加视频轨道")
            mIsVideoTrackAdd = true
            startMuxer()
        }
    }

    fun addAudioTrack(mediaFormat: MediaFormat) {
        if (mIsAudioTrackAdd) return
        if (mMediaMuxer != null) {
            mAudioTrackIndex = try {
                mMediaMuxer!!.addTrack(mediaFormat)
            } catch (e: Exception) {
                e.printStackTrace()
                return
            }
            Log.i(TAG, "添加音频轨道")
            mIsAudioTrackAdd = true
            startMuxer()
        }
    }

    fun setNoAudio() {
        if (mIsAudioTrackAdd) return
        mIsAudioTrackAdd = true
        mIsAudioEnd = true
        startMuxer()
    }

    fun setNoVideo() {
        if (mIsVideoTrackAdd) return
        mIsVideoTrackAdd = true
        mIsVideoEnd = true
        startMuxer()
    }

    fun writeVideoData(byteBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo) {
        if (mIsStart) {
            mMediaMuxer?.writeSampleData(mVideoTrackIndex, byteBuffer, bufferInfo)
        }
    }

    fun writeAudioData(byteBuffer: ByteBuffer, bufferInfo: MediaCodec.BufferInfo) {
        if (mIsStart) {
            mMediaMuxer?.writeSampleData(mAudioTrackIndex, byteBuffer, bufferInfo)
        }
    }

    private fun startMuxer() {
        if (mIsAudioTrackAdd && mIsVideoTrackAdd) {
            mMediaMuxer?.start()
            mIsStart = true
            mStateListener?.onMuxerStart()
            Log.i(TAG, "启动封装器")
        }
    }

    fun releaseVideoTrack() {
        mIsVideoEnd = true
        release()
    }

    fun releaseAudioTrack() {
        mIsAudioEnd = true
        release()
    }

    private fun release() {
        if (mIsAudioEnd && mIsVideoEnd) {
            mIsAudioTrackAdd = false
            mIsVideoTrackAdd = false
            try {
                mMediaMuxer?.stop()
                mMediaMuxer?.release()
                mMediaMuxer = null
                Log.i(TAG, "退出封装器")
            } catch (e: Exception) {
                e.printStackTrace()
            } finally {
                mStateListener?.onMuxerFinish()
            }
        }
    }

    fun setStateListener(l: IMuxerStateListener) {
        this.mStateListener = l
    }

    interface IMuxerStateListener {
        fun onMuxerStart() {}
        fun onMuxerFinish() {}
    }
}