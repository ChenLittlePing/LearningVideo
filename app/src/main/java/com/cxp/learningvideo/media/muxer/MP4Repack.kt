package com.cxp.learningvideo.media.muxer

import android.media.MediaCodec
import android.util.Log
import com.cxp.learningvideo.media.extractor.AudioExtractor
import com.cxp.learningvideo.media.extractor.VideoExtractor
import java.nio.ByteBuffer


/**
 * MP4重打包工具
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-09-19 14:09
 *
 */
class MP4Repack(path: String) {

    private val TAG = "MP4Repack"

    private val mAExtractor: AudioExtractor = AudioExtractor(path)
    private val mVExtractor: VideoExtractor = VideoExtractor(path)
    private val mMuxer: MMuxer = MMuxer()

    fun start() {
        val audioFormat = mAExtractor.getFormat()
        val videoFormat = mVExtractor.getFormat()

        if (audioFormat != null) {
            mMuxer.addAudioTrack(audioFormat)
        } else {
            mMuxer.setNoAudio()
        }
        if (videoFormat != null) {
            mMuxer.addVideoTrack(videoFormat)
        } else {
            mMuxer.setNoVideo()
        }

        Thread {
            val buffer = ByteBuffer.allocate(500 * 1024)
            val bufferInfo = MediaCodec.BufferInfo()
            if (audioFormat != null) {
                var size = mAExtractor.readBuffer(buffer)
                while (size > 0) {
                    bufferInfo.set(0, size, mAExtractor.getCurrentTimestamp(), mAExtractor.getSampleFlag())
                    mMuxer.writeAudioData(buffer, bufferInfo)
                    size = mAExtractor.readBuffer(buffer)
                }
            }
            if (videoFormat != null) {
                var size = mVExtractor.readBuffer(buffer)
                while (size > 0) {
                    bufferInfo.set(0, size, mVExtractor.getCurrentTimestamp(), mVExtractor.getSampleFlag())
                    mMuxer.writeVideoData(buffer, bufferInfo)
                    size = mVExtractor.readBuffer(buffer)
                }
            }
            mAExtractor.stop()
            mVExtractor.stop()
            mMuxer.releaseAudioTrack()
            mMuxer.releaseVideoTrack()
            Log.i(TAG, "MP4 重打包完成")
        }.start()
    }
}