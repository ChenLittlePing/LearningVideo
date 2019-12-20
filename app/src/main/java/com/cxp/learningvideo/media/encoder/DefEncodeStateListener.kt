package com.cxp.learningvideo.media.encoder


/**
 * 默认编码回调接口
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 *
 */
interface DefEncodeStateListener: IEncodeStateListener {

    override fun encodeStart(encoder: BaseEncoder) {

    }

    override fun encodeProgress(encoder: BaseEncoder) {

    }

    override fun encoderFinish(encoder: BaseEncoder) {

    }
}