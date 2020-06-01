//
// OpenGL EGL窗口
// Created by cxp on 2019-08-05.
//

#include "egl_surface.h"

EglSurface::EglSurface() {
    m_core = new EglCore();
}

EglSurface::~EglSurface() {
    delete m_core;
}

bool EglSurface::Init() {
    return m_core->Init(NULL);
}

void EglSurface::CreateEglSurface(ANativeWindow *native_window, int width, int height) {
    if (native_window != NULL) {
        this->m_native_window = native_window;
        m_surface = m_core->CreateWindSurface(m_native_window);
    } else {
        m_surface = m_core->CreateOffScreenSurface(width, height);
    }
    if (m_surface == NULL) {
        LOGE(TAG, "EGL create window surface fail")
        Release();
    }
    MakeCurrent();
}

void EglSurface::SwapBuffers() {
    m_core->SwapBuffers(m_surface);
}

void EglSurface::MakeCurrent() {
    m_core->MakeCurrent(m_surface);
}

void EglSurface::DestroyEglSurface() {
    if (m_surface != NULL) {
        if (m_core != NULL) {
            m_core->DestroySurface(m_surface);
        }
        m_surface = NULL;
    }
}

void EglSurface::Release() {
    DestroyEglSurface();
    if (m_core != NULL) {
        m_core->Release();
    }
}