//
// 音频编码器
// Author: Chen Xiaoping
// Create Date: 2019-11-25.
//

#include "a_encoder.h"
#include "../../const.h"

AudioEncoder::AudioEncoder(JNIEnv *env, Mp4Muxer *muxer)
: BaseEncoder(env, muxer, AV_CODEC_ID_AAC) {

}

void AudioEncoder::InitContext(AVCodecContext *codec_ctx) {
    codec_ctx->codec_id = AV_CODEC_ID_AAC;
    codec_ctx->codec_type = AVMEDIA_TYPE_AUDIO;
    codec_ctx->sample_fmt = ENCODE_AUDIO_DEST_FORMAT;
    codec_ctx->sample_rate = ENCODE_AUDIO_DEST_SAMPLE_RATE;
    codec_ctx->channel_layout = ENCODE_AUDIO_DEST_CHANNEL_LAYOUT;
    codec_ctx->channels = ENCODE_AUDIO_DEST_CHANNEL_COUNTS;
    codec_ctx->bit_rate = ENCODE_AUDIO_DEST_BIT_RATE;

    InitFrame();
}

void AudioEncoder::InitFrame() {
    m_frame = av_frame_alloc();
    m_frame->nb_samples = 1024;
    m_frame->format = ENCODE_AUDIO_DEST_FORMAT;
    m_frame->channel_layout = ENCODE_AUDIO_DEST_CHANNEL_LAYOUT;

    int size = av_samples_get_buffer_size(NULL, ENCODE_AUDIO_DEST_CHANNEL_COUNTS, m_frame->nb_samples,
                                          ENCODE_AUDIO_DEST_FORMAT, 1);
    uint8_t *frame_buf = (uint8_t *) av_malloc(size);
    avcodec_fill_audio_frame(m_frame, ENCODE_AUDIO_DEST_CHANNEL_COUNTS, ENCODE_AUDIO_DEST_FORMAT,
                             frame_buf, size, 1);
    LOGE("AudioEncoder", "InitFrame size : %d", size)
}

int AudioEncoder::ConfigureMuxerStream(Mp4Muxer *muxer, AVCodecContext *ctx) {
    return muxer->AddAudioStream(ctx);
}

AVFrame* AudioEncoder::DealFrame(OneFrame *one_frame) {
    m_frame->pts = one_frame->pts;
    memcpy(m_frame->data[0], one_frame->data, 4096);
    memcpy(m_frame->data[1], one_frame->ext_data, 4096);
    return m_frame;
}

void AudioEncoder::Release() {
    m_muxer->EndAudioStream();
}