package com.cxp.learningvideo.media


/**
 * 解码状态
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-09-02 10:00
 *
 */
enum class DecodeState {
    /**开始状态*/
    START,
    /**解码中*/
    DECODING,
    /**解码暂停*/
    PAUSE,
    /**正在快进*/
    SEEKING,
    /**解码完成*/
    FINISH,
    /**解码器释放*/
    STOP
}
