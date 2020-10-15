//
// 使用OpenGL渲染画面的音视频合成器
// Author: Chen Xiaoping
// Create Date: 2019-10-31.
//

#include "synthesizer.h"
#include "../../opengl/drawer/proxy/def_drawer_proxy_impl.h"
#include "../../opengl/drawer/video_drawer.h"

static int WIDTH = 1920;
static int HEIGHT = 1080;

Synthesizer::Synthesizer(JNIEnv *env, jstring src_path, jstring dst_path) {

    // 封装器
    m_mp4_muxer = new Mp4Muxer();
    m_mp4_muxer->Init(env, dst_path);
    m_mp4_muxer->SetMuxFinishCallback(this);

    // --------------------------视频配置--------------------------
    // 视频编码器
    m_v_encoder = new VideoEncoder(env, m_mp4_muxer, WIDTH, HEIGHT);
    m_v_encoder->SetStateReceiver(this);

    // 绘制器
    m_drawer_proxy = new DefDrawerProxyImpl();
    VideoDrawer *drawer = new VideoDrawer();
    m_drawer_proxy->AddDrawer(drawer);

    // OpenGL 渲染器
    m_gl_render = new OpenGLRender(env, m_drawer_proxy);
    m_gl_render->SetOffScreenSize(WIDTH, HEIGHT);
    m_gl_render->SetPixelReceiver(this); // 接收经过（编辑）渲染的画面数据

    // 视频解码器
    m_video_decoder = new VideoDecoder(env, src_path, true);
    m_video_decoder->SetRender(drawer);

    // 监听解码状态
    m_video_decoder->SetStateReceiver(this);

    //--------------------------音频配置--------------------------
    // 音频编码器
    m_a_encoder = new AudioEncoder(env, m_mp4_muxer);
    // 监听编码状态
    m_a_encoder->SetStateReceiver(this);

    // 音频解码器
    m_audio_decoder = new AudioDecoder(env, src_path, true);
    // 监听解码状态
    m_audio_decoder->SetStateReceiver(this);
}

Synthesizer::~Synthesizer() {
}

void Synthesizer::Start() {
    m_video_decoder->GoOn();
    m_audio_decoder->GoOn();
}

void Synthesizer::DecodePrepare(IDecoder *decoder) {

}

void Synthesizer::DecodeReady(IDecoder *decoder) {
}

void Synthesizer::DecodeRunning(IDecoder *decoder) {

}

void Synthesizer::DecodePause(IDecoder *decoder) {

}

bool Synthesizer::DecodeOneFrame(IDecoder *decoder, OneFrame *frame) {
    if (decoder == m_video_decoder) {
        while (m_cur_v_frame) {
            av_usleep(2000);
        }
        m_cur_v_frame = frame;
        m_gl_render->RequestRgbaData();
        return m_v_encoder->TooMuchData();
    } else {
        m_cur_a_frame = frame;
        m_a_encoder->PushFrame(frame);
        return m_a_encoder->TooMuchData();
    }
}

void Synthesizer::ReceivePixel(uint8_t *rgba) {
    OneFrame *rgbFrame = new OneFrame(rgba, m_cur_v_frame->line_size,
                                      m_cur_v_frame->pts, m_cur_v_frame->time_base);
    m_v_encoder->PushFrame(rgbFrame);
    m_cur_v_frame = NULL;
}

void Synthesizer::DecodeFinish(IDecoder *decoder) {
    // 编码结束，压入一帧空数据，通知编码器结束编码
    if (decoder == m_video_decoder) {
        m_v_encoder->PushFrame(new OneFrame(NULL, 0, 0, AVRational{1, 25}, NULL));
    } else {
        m_a_encoder->PushFrame(new OneFrame(NULL, 0, 0, AVRational{1, 25}, NULL));
    }
}

void Synthesizer::DecodeStop(IDecoder *decoder) {
}

void Synthesizer::EncodeStart() {

}

void Synthesizer::EncodeSend() {
}

void Synthesizer::EncodeFrame(void *data) {
}

void Synthesizer::EncodeProgress(long time) {

}

void Synthesizer::EncodeFinish() {
    LOGI("Synthesizer", "EncodeFinish ...");
}

void Synthesizer::OnMuxFinished() {
    LOGI("Synthesizer", "OnMuxFinished ...");
    m_gl_render->Stop();

    if (m_mp4_muxer != NULL) {
        delete m_mp4_muxer;
    }
    m_drawer_proxy = NULL;
}