//
// 音频编码器
// Author: Chen Xiaoping
// Create Date: 2019-11-25.
//

#ifndef LEARNVIDEO_AUDIO_ENCODER_H
#define LEARNVIDEO_AUDIO_ENCODER_H


#include "../base_encoder.h"

extern "C" {
#include <libswresample/swresample.h>
};

class AudioEncoder: public BaseEncoder {

private:
    AVFrame *m_frame = NULL;

    void InitFrame();

protected:
    void InitContext(AVCodecContext *codec_ctx) override;

    int ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) override;

    AVFrame* DealFrame(OneFrame *one_frame) override;

    void Release() override;

    const char *const LogSpec() override {
        return "音频";
    };

public:
    AudioEncoder(JNIEnv *env, Mp4Muxer *muxer);
};


#endif //LEARNVIDEO_AUDIO_ENCODER_H
