package com.cxp.learningvideo.media

import android.media.MediaFormat
import java.nio.ByteBuffer


/**
 * 音视频分离器定义
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-09-02 10:07
 *
 */
interface IExtractor {

    fun getFormat(): MediaFormat?

    /**
     * 读取音视频数据
     */
    fun readBuffer(byteBuffer: ByteBuffer): Int

    /**
     * 获取当前帧时间
     */
    fun getCurrentTimestamp(): Long

    fun getSampleFlag(): Int

    /**
     * Seek到指定位置，并返回实际帧的时间戳
     */
    fun seek(pos: Long): Long

    fun setStartPos(pos: Long)

    /**
     * 停止读取数据
     */
    fun stop()
}