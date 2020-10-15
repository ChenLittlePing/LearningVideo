//
// Mp4混合器
// Author: Chen Xiaoping
// Create Date: 2019-10-31.
//

#ifndef LEARNVIDEO_MP4_MUXER_H
#define LEARNVIDEO_MP4_MUXER_H

#include <jni.h>
#include "i_muxer_cb.h"

extern "C" {
#include <libavformat/avformat.h>
};

typedef void (*mux_finish_cb)();

class Mp4Muxer {
private:

    const char *TAG = "Mp4Muxer";

    char *m_path;

    AVFormatContext * m_fmt_ctx = NULL;

    bool m_audio_configured = false;

    bool m_audio_end = false;

    bool m_video_configured = false;

    bool m_video_end = false;

    IMuxerCb *m_mux_finish_cb = NULL;

    int AddStream(AVCodecContext *ctx);

public:
    Mp4Muxer();

    ~Mp4Muxer();

    void SetMuxFinishCallback(IMuxerCb *cb) {
        this->m_mux_finish_cb = cb;
    }

    AVRational GetTimeBase(int stream_index) {
        return m_fmt_ctx->streams[stream_index]->time_base;
    }

    void Init(JNIEnv *env, jstring path);

    int AddVideoStream(AVCodecContext *ctx);
    int AddAudioStream(AVCodecContext *ctx);

    void Start();

    void Write(AVPacket *pkt);

    void EndVideoStream();

    void EndAudioStream();

    void Release();
};


#endif //LEARNVIDEO_MP4_MUXER_H
