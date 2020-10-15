//
// 视频编码器
// Author: Chen Xiaoping
// Create Date: 2019-11-14.
//

#include "v_encoder.h"
#include "../../const.h"

VideoEncoder::VideoEncoder(JNIEnv *env, Mp4Muxer *muxer, int width, int height)
: BaseEncoder(env, muxer, AV_CODEC_ID_H264),
m_width(width),
m_height(height) {
    m_sws_ctx = sws_getContext(width, height, AV_PIX_FMT_RGBA,
                               width, height, AV_PIX_FMT_YUV420P, SWS_FAST_BILINEAR,
                               NULL, NULL, NULL);
}

void VideoEncoder::InitContext(AVCodecContext *codec_ctx) {

    codec_ctx->bit_rate = 3*m_width*m_height;

    codec_ctx->width = m_width;
    codec_ctx->height = m_height;

    //把1秒钟分成fps个单位
    codec_ctx->time_base = {1, ENCODE_VIDEO_FPS};
    codec_ctx->framerate = {ENCODE_VIDEO_FPS, 1};

    //画面组大小
    codec_ctx->gop_size = 50;
    //没有B帧
    codec_ctx->max_b_frames = 0;

    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
//    codec_ctx->codec_id = AV_CODEC_ID_H264;

    codec_ctx->thread_count = 8;

    av_opt_set(codec_ctx->priv_data, "preset", "ultrafast", 0);
    av_opt_set(codec_ctx->priv_data, "tune", "zerolatency", 0);

    //这是量化范围设定，其值范围为0~51，
    //越小质量越高，需要的比特率越大，0为无损编码
    codec_ctx->qmin = 28;
    codec_ctx->qmax = 50;

    //全局的编码信息
    codec_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    InitYUVFrame();

    LOGI(TAG, "Init codec context success")
}

void VideoEncoder::InitYUVFrame() {
    //设置YUV输出空间
    m_yuv_frame = av_frame_alloc();
    m_yuv_frame->format = AV_PIX_FMT_YUV420P;
    m_yuv_frame->width = m_width;
    m_yuv_frame->height = m_height;
    //分配空间
    int ret = av_frame_get_buffer(m_yuv_frame, 0);
    if (ret < 0) {
        LOGE(TAG, "Fail to get yuv frame buffer");
    }
}

int VideoEncoder::ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) {
    return muxer->AddVideoStream(ctx);
}

AVFrame* VideoEncoder::DealFrame(OneFrame *one_frame) {
    uint8_t *in_data[AV_NUM_DATA_POINTERS] = { 0 };
    in_data[0] = one_frame->data;
    int src_line_size[AV_NUM_DATA_POINTERS] = { 0 };
    src_line_size[0] = one_frame->line_size;

    int h = sws_scale(m_sws_ctx, in_data, src_line_size, 0, m_height,
                      m_yuv_frame->data, m_yuv_frame->linesize);
    if (h <= 0) {
        LOGE(TAG, "转码出错");
        return NULL;
    }

    m_yuv_frame->pts = one_frame->pts;

    return m_yuv_frame;
}

void VideoEncoder::Release() {
    if (m_yuv_frame != NULL) {
        av_frame_free(&m_yuv_frame);
        m_yuv_frame = NULL;
    }
    if (m_sws_ctx != NULL) {
        sws_freeContext(m_sws_ctx);
        m_sws_ctx = NULL;
    }
    m_muxer->EndVideoStream();
}

