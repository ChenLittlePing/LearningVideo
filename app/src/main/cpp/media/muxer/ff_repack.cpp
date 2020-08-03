//
// Created by cxp on 2020-08-01.
//

#include <unistd.h>
#include "ff_repack.h"
#include "../../utils/logger.h"

FFRepack::FFRepack(JNIEnv *env, jstring in_path, jstring out_path) {
    const char *srcPath = env->GetStringUTFChars(in_path, NULL);
    const char *destPath = env->GetStringUTFChars(out_path, NULL);

    // 打开原视频文件，并获取相关参数
    if (OpenSrcFile(srcPath) >= 0) {
        LOGE(TAG, "Open src file success")
        // 初始化打包参数
        if (InitMuxerParams(destPath) >= 0) {
            LOGE(TAG, "Init muxer params success")
        } else {
            LOGE(TAG, "Init muxer params fail")
        }
    } else {
        LOGE(TAG, "Open src file fail")
    }
}

int FFRepack::OpenSrcFile(const char *srcPath) {
    // 打开文件
    LOGI(TAG, "Open file: %s", srcPath)
    if ((avformat_open_input(&m_in_format_cxt, srcPath, NULL, NULL)) < 0) {
        LOGE(TAG, "Fail to open input file")
        return -1;
    }

    // 获取音视频参数
    if ((avformat_find_stream_info(m_in_format_cxt, 0)) < 0) {
        LOGE(TAG, "Fail to retrieve input stream information")
        return -1;
    }

    return 0;
}

int FFRepack::InitMuxerParams(const char *destPath) {
    // 初始化输出上下文
    if (avformat_alloc_output_context2(&m_out_format_cxt, NULL, NULL, destPath) < 0) {
        return -1;
    }

    // 查找原视频所有媒体流
    for (int i = 0; i < m_in_format_cxt->nb_streams; ++i) {
        // 获取媒体流
        AVStream *in_stream = m_in_format_cxt->streams[i];

        // 为目标文件创建输出流
        AVStream *out_stream = avformat_new_stream(m_out_format_cxt, NULL);
        if (!out_stream) {
            LOGE(TAG, "Fail to allocate output stream")
            return -1;
        }

        // 复制原视频数据流参数到目标输出流
        if (avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar) < 0) {
            LOGE(TAG, "Fail to copy input context to output stream")
            return -1;
        }
    }

    // 打开目标文件
    if (avio_open(&m_out_format_cxt->pb, destPath, AVIO_FLAG_WRITE) < 0) {
        LOGE(TAG, "Could not open output file %s ", destPath);
        return -1;
    }

    // 写入文件头信息
    if (avformat_write_header(m_out_format_cxt, NULL) < 0) {
        LOGE(TAG, "Error occurred when opening output file");
        return -1;
    } else {
        LOGE(TAG, "Write file header success");
    }

    return 0;
}

void FFRepack::Start() {
    LOGE(TAG, "Start repacking ....")
    AVPacket pkt;
    while (1) {
        // 读取数据
        if (av_read_frame(m_in_format_cxt, &pkt)) {
            LOGE(TAG, "End of video，write trailer")

            // 释放数据帧和相关资源
            av_packet_unref(&pkt);

            // 读取完毕，写入结尾信息
            av_write_trailer(m_out_format_cxt);

            break;
        }

        // 写入一帧数据
        Write(pkt);

        // 这里先不要释放资源，否则会导致写入异常，在文件读取完毕后释放
        // av_packet_unref(&pkt);
    }

    // 释放资源
    Release();
}

void FFRepack::Write(AVPacket pkt) {

    // 获取数据对应的输入/输出流
    AVStream *in_stream = m_in_format_cxt->streams[pkt.stream_index];
    AVStream *out_stream = m_out_format_cxt->streams[pkt.stream_index];

    // 转换时间基对应的 PTS/DTS
    int rounding = (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
    pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                               (AVRounding)rounding);
    pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                               (AVRounding)rounding);

    pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);

    pkt.pos = -1;

    // 将数据写入目标文件

    int ret = av_interleaved_write_frame(m_out_format_cxt, &pkt);
    if (ret < 0) {
        LOGE(TAG, "Error to muxing packet: %x", ret)
    }
}

void FFRepack::Release() {
    LOGE(TAG, "Finish repacking, release resources")
    // 关闭输入
    if (m_in_format_cxt) {
        avformat_close_input(&m_in_format_cxt);
    }

    // 关闭输出
    if (m_out_format_cxt) {
        avio_close(m_out_format_cxt->pb);
        avformat_free_context(m_out_format_cxt);
    }
}