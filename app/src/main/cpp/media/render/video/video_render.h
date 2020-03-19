//
// Created by cxp on 2019-08-06.
//

#ifndef LEARNVIDEO_VIDEORENDER_H
#define LEARNVIDEO_VIDEORENDER_H

#include <stdint.h>
#include <jni.h>

#include "../../one_frame.h"

class VideoRender {
public:
    virtual void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) = 0;
    virtual void Render(OneFrame *one_frame) = 0;
    virtual void ReleaseRender() = 0;
};


#endif //LEARNVIDEO_VIDEORENDER_H
