package com.cxp.learningvideo.utils

import android.media.MediaCodecInfo
import android.media.MediaCodecList
import android.util.Log


/**
 * 测试工具
 *
 * @author Chen Xiaoping (562818444@qq.com)
 * @since LearningVideo
 * @version LearningVideo
 * @Datetime 2019-10-11 11:06
 *
 */
fun printMediaCodecInfo() {
    var codecCount = 0
    try {
        codecCount = MediaCodecList.getCodecCount()
    } catch (e: Exception) {
        Log.e("cccccc", "##### Failed to get codec count!")
        e.printStackTrace()
        return
    }

    for (i in 0 until codecCount) {
        var info: MediaCodecInfo? = null
        try {
            info = MediaCodecList.getCodecInfoAt(i)
        } catch (e: IllegalArgumentException) {
            Log.e("cccccc", "Cannot retrieve decoder codec info", e)
        }

        if (info == null) {
            continue
        }

        var codecInfo = "MediaCodec, name=" + info.name + ", ["

        for (mimeType in info.supportedTypes) {
            codecInfo += "$mimeType,"
            val capabilities: MediaCodecInfo.CodecCapabilities
            try {
                capabilities = info.getCapabilitiesForType(mimeType)
            } catch (e: IllegalArgumentException) {
                Log.e("cccccc", "Cannot retrieve decoder capabilities", e)
                continue
            }

            codecInfo += " max inst:" + capabilities.maxSupportedInstances + ","

            var strColorFormatList = ""
            for (colorFormat in capabilities.colorFormats) {
                strColorFormatList += " 0x" + Integer.toHexString(colorFormat)
            }
            codecInfo += "$strColorFormatList] ["
        }
        Log.w("cccccc", codecInfo)
    }

}