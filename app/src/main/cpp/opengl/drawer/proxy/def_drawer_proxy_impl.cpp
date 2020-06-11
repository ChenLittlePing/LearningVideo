//
// 默认画面渲染代理
// Created by cxp on 2019-08-08.
//

#include "def_drawer_proxy_impl.h"
#include "../../../utils/logger.h"

void DefDrawerProxyImpl::AddDrawer(Drawer *drawer) {
    m_drawers.push_back(drawer);
}

void DefDrawerProxyImpl::Draw() {
    for (int i = 0; i < m_drawers.size(); ++i) {
        m_drawers[i]->Draw();
    }
}

void DefDrawerProxyImpl::Release() {
    for (int i = 0; i < m_drawers.size(); ++i) {
        m_drawers[i]->Release();
        delete m_drawers[i];
    }

    m_drawers.clear();
}