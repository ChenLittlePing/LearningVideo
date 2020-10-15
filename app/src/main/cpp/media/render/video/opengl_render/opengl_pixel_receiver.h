//
// Created by cxp on 2019-11-14.
//

#ifndef LEARNVIDEO_OPENGL_PIXEL_OUTPUT_H
#define LEARNVIDEO_OPENGL_PIXEL_OUTPUT_H

#include <stdint.h>

class OpenGLPixelReceiver {
public:
    virtual void ReceivePixel(uint8_t *rgba) = 0;
};

#endif //LEARNVIDEO_OPENGL_PIXEL_OUTPUT_H
