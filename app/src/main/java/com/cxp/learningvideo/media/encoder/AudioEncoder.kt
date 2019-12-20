package com.cxp.learningvideo.media.encoder

import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.os.Build
import android.util.Log
import com.cxp.learningvideo.media.muxer.MMuxer
import java.nio.ByteBuffer


/**
 * 视频编码器
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 *
 */

// 编码采样率率
val DEST_SAMPLE_RATE = 44100
// 编码码率
private val DEST_BIT_RATE = 128000

class AudioEncoder(muxer: MMuxer): BaseEncoder(muxer) {

    private val TAG = "AudioEncoder"

    override fun encodeType(): String {
        return "audio/mp4a-latm"
    }

    override fun configEncoder(codec: MediaCodec) {
        val audioFormat = MediaFormat.createAudioFormat(encodeType(), DEST_SAMPLE_RATE, 2)
        audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, DEST_BIT_RATE)
        audioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 100*1024)
        try {
            configEncoderWithCQ(codec, audioFormat)
        } catch (e: Exception) {
            e.printStackTrace()
            try {
                configEncoderWithVBR(codec, audioFormat)
            } catch (e: Exception) {
                e.printStackTrace()
                Log.e(TAG, "配置音频编码器失败")
            }
        }
    }

    private fun configEncoderWithCQ(codec: MediaCodec, outputFormat: MediaFormat) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            // 本部分手机不支持 BITRATE_MODE_CQ 模式，有可能会异常
            outputFormat.setInteger(
                MediaFormat.KEY_BITRATE_MODE,
                MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_CQ
            )
        }
        codec.configure(outputFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
    }

    private fun configEncoderWithVBR(codec: MediaCodec, outputFormat: MediaFormat) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            outputFormat.setInteger(
                MediaFormat.KEY_BITRATE_MODE,
                MediaCodecInfo.EncoderCapabilities.BITRATE_MODE_VBR
            )
        }
        codec.configure(outputFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
    }

    override fun addTrack(muxer: MMuxer, mediaFormat: MediaFormat) {
        muxer.addAudioTrack(mediaFormat)
    }

    override fun frameWaitTimeMs(): Long {
        return 5
    }

    override fun writeData(
        muxer: MMuxer,
        byteBuffer: ByteBuffer,
        bufferInfo: MediaCodec.BufferInfo
    ) {
        muxer.writeAudioData(byteBuffer, bufferInfo)
    }

    override fun release(muxer: MMuxer) {
        muxer.releaseAudioTrack()
    }
}