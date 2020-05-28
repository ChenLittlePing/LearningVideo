package com.cxp.learningvideo.media


/**
 * 默认实现的解码监听器
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2020-05-28 20:18
 *
 */
interface DefDecoderStateListener: IDecoderStateListener {
    override fun decoderPrepare(decodeJob: BaseDecoder?) {}

    override fun decoderReady(decodeJob: BaseDecoder?) {}

    override fun decoderRunning(decodeJob: BaseDecoder?) {}

    override fun decoderPause(decodeJob: BaseDecoder?) {}

    override fun decodeOneFrame(decodeJob: BaseDecoder?, frame: Frame) {}

    override fun decoderFinish(decodeJob: BaseDecoder?) {}

    override fun decoderDestroy(decodeJob: BaseDecoder?) {}

    override fun decoderError(decodeJob: BaseDecoder?, msg: String) {}
}