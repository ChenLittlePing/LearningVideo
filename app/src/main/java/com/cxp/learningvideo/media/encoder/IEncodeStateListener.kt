package com.cxp.learningvideo.media.encoder


/**
 * 编码状态回调接口
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 *
 */
interface IEncodeStateListener {
    fun encodeStart(encoder: BaseEncoder)
    fun encodeProgress(encoder: BaseEncoder)
    fun encoderFinish(encoder: BaseEncoder)
}