//
// Created by cxp on 2019-08-08.
//

#ifndef LEARNVIDEO_DRAWER_PROXY_H
#define LEARNVIDEO_DRAWER_PROXY_H


#include "../drawer.h"

class DrawerProxy {
public:
    virtual void AddDrawer(Drawer *drawer) = 0;
    virtual void Draw() = 0;
    virtual void Release() = 0;
    virtual ~DrawerProxy() {}
};


#endif //LEARNVIDEO_DRAWER_PROXY_H
