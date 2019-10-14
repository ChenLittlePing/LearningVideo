package com.cxp.learningvideo.opengl.drawer


/**
 * 渲染器
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-10-12 10:04
 *
 */
interface IDrawer {
    fun draw()
    fun setTextureID(id: Int)
    fun release()
}