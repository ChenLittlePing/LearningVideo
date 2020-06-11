//
// OpenGL EGL核心配置
// Created by cxp on 2019-08-05.
//

#include "egl_core.h"

EglCore::EglCore() {
}

EglCore::~EglCore() {

}

bool EglCore::Init(EGLContext share_ctx) {
    if (m_egl_dsp != EGL_NO_DISPLAY) {
        LOGE(TAG, "EGL already set up")
        return true;
    }

    if (share_ctx == NULL) {
        share_ctx = EGL_NO_CONTEXT;
    }

    m_egl_dsp = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (m_egl_dsp == EGL_NO_DISPLAY || eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL init display fail")
        return false;
    }

    EGLint major_ver, minor_ver;
    EGLBoolean success = eglInitialize(m_egl_dsp, &major_ver, &minor_ver);
    if (success != EGL_TRUE || eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL init fail")
        return false;
    }

    LOGI(TAG, "EGL version: %d.%d", major_ver, minor_ver)

    m_egl_cfg = GetEGLConfig();

    const EGLint attr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    m_egl_cxt = eglCreateContext(m_egl_dsp, m_egl_cfg, share_ctx, attr);
    if (m_egl_cxt == EGL_NO_CONTEXT) {
        LOGE(TAG, "EGL create fail, error is %x", eglGetError());
        return false;
    }

    EGLint egl_format;
    success = eglGetConfigAttrib(m_egl_dsp, m_egl_cfg, EGL_NATIVE_VISUAL_ID, &egl_format);
    if (success != EGL_TRUE || eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL get config fail")
        return false;
    }

    LOGI(TAG, "EGL init success")
    return true;
}

EGLConfig EglCore::GetEGLConfig() {
    EGLint numConfigs;
    EGLConfig config;
    static const EGLint CONFIG_ATTRIBS[] = { EGL_BUFFER_SIZE, EGL_DONT_CARE,
                                             EGL_RED_SIZE, 8,
                                             EGL_GREEN_SIZE, 8,
                                             EGL_BLUE_SIZE, 8,
                                             EGL_ALPHA_SIZE, 8,
                                             EGL_DEPTH_SIZE, 16,
                                             EGL_STENCIL_SIZE, EGL_DONT_CARE,
                                             EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                                             EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                                             EGL_NONE // the end
    };

    EGLBoolean success = eglChooseConfig(m_egl_dsp, CONFIG_ATTRIBS, &config, 1, &numConfigs);
    if (!success || eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL config fail")
        return NULL;
    }
    return config;
}

EGLSurface EglCore::CreateWindSurface(ANativeWindow *window) {
    EGLSurface surface = eglCreateWindowSurface(m_egl_dsp, m_egl_cfg, window, 0);
    if (eglGetError() != EGL_SUCCESS) {
        LOGI(TAG, "EGL create window surface fail")
        return NULL;
    }
    return surface;
}

EGLSurface EglCore::CreateOffScreenSurface(int width, int height) {
    int CONFIG_ATTRIBS[] = {
            EGL_WIDTH, width,
            EGL_HEIGHT, height,
            EGL_NONE
    };

    EGLSurface surface = eglCreatePbufferSurface(m_egl_dsp, m_egl_cfg, CONFIG_ATTRIBS);
    if (eglGetError() != EGL_SUCCESS) {
        LOGI(TAG, "EGL create off screen surface fail")
        return NULL;
    }
    return surface;
}

void EglCore::MakeCurrent(EGLSurface egl_surface) {
    if (!eglMakeCurrent(m_egl_dsp, egl_surface, egl_surface, m_egl_cxt)) {
        LOGE(TAG, "EGL make current fail");
    }
}

void EglCore::SwapBuffers(EGLSurface egl_surface) {
    eglSwapBuffers(m_egl_dsp, egl_surface);
}

void EglCore::DestroySurface(EGLSurface elg_surface) {
    eglMakeCurrent(m_egl_dsp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(m_egl_dsp, elg_surface);
}

void EglCore::Release() {
    if (m_egl_dsp != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_egl_dsp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_egl_dsp, m_egl_cxt);
        eglReleaseThread();
        eglTerminate(m_egl_dsp);
    }
    m_egl_dsp = EGL_NO_DISPLAY;
    m_egl_cxt = EGL_NO_CONTEXT;
    m_egl_cfg = NULL;
}

