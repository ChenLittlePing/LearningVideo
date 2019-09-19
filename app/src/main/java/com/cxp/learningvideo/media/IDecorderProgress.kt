package com.cxp.learningvideo.media


/**
 * 解码进度
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-09-02 09:54
 *
 */
interface IDecoderProgress {
    /**
     * 视频宽高回调
     */
    fun videoSizeChange(width: Int, height: Int, rotationAngle: Int)

    /**
     * 视频播放进度回调
     */
    fun videoProgressChange(pos: Long)
}