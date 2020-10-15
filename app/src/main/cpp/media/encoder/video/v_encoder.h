//
// 视频编码器
// Author: Chen Xiaoping
// Create Date: 2019-11-14.
//

#ifndef LEARNVIDEO_V_ENCODER_H
#define LEARNVIDEO_V_ENCODER_H


#include "../base_encoder.h"

class VideoEncoder: public BaseEncoder {
private:

    const char * TAG = "VideoEncoder";

    SwsContext *m_sws_ctx = NULL;

    AVFrame *m_yuv_frame = NULL;

    int m_width = 0, m_height = 0;

    void InitYUVFrame();

protected:

    const char *const LogSpec() override {
        return "视频";
    };

    void InitContext(AVCodecContext *codec_ctx) override;
    int ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) override;
    AVFrame* DealFrame(OneFrame *one_frame) override;
    void Release() override;

public:
    VideoEncoder(JNIEnv *env, Mp4Muxer *muxer, int width, int height);

};


#endif //LEARNVIDEO_V_ENCODER_H
