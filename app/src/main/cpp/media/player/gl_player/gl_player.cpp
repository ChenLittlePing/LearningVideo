//
// Created by cxp on 2020-05-31.
//

#include "gl_player.h"

#include "../../render/video/native_render/native_render.h"
#include "../../render/audio/opensl_render.h"
#include "../../render/video/opengl_render/opengl_render.h"
#include "../../../opengl/drawer/proxy/def_drawer_proxy_impl.h"

GLPlayer::GLPlayer(JNIEnv *jniEnv, jstring path) {
    m_v_decoder = new VideoDecoder(jniEnv, path);

    // OpenGL 渲染
    m_v_drawer = new VideoDrawer();
    m_v_decoder->SetRender(m_v_drawer);

    DefDrawerProxyImpl *proxyImpl =  new DefDrawerProxyImpl();
    proxyImpl->AddDrawer(m_v_drawer);

    m_v_drawer_proxy = proxyImpl;

    m_gl_render = new OpenGLRender(jniEnv, m_v_drawer_proxy);

    // 音频解码
    m_a_decoder = new AudioDecoder(jniEnv, path, false);
    m_a_render = new OpenSLRender();
    m_a_decoder->SetRender(m_a_render);
}

GLPlayer::~GLPlayer() {
    // 此处不需要 delete 成员指针
    // 在BaseDecoder 和 OpenGLRender 中的线程已经使用智能指针，会自动释放相关指针
}

void GLPlayer::SetSurface(jobject surface) {
    m_gl_render->SetSurface(surface);
}

void GLPlayer::PlayOrPause() {
    if (!m_v_decoder->IsRunning()) {
        LOGI("Player", "播放视频")
        m_v_decoder->GoOn();
    } else {
        LOGI("Player", "暂停视频")
        m_v_decoder->Pause();
    }
    if (!m_a_decoder->IsRunning()) {
        LOGI("Player", "播放音频")
        m_a_decoder->GoOn();
    } else {
        LOGI("Player", "暂停音频")
        m_a_decoder->Pause();
    }
}

void GLPlayer::Release() {
    m_gl_render->Stop();
    m_v_decoder->Stop();
    m_a_decoder->Stop();
}