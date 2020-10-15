//
// 编码回调接口定义
// Author: Chen Xiaoping
// Create Date: 2019-11-14.
//


#ifndef LEARNVIDEO_DECODE_STATE_CB_H
#define LEARNVIDEO_DECODE_STATE_CB_H

#include "../one_frame.h"
//声明IDecoder，在cpp中include，编码重复引用
class IDecoder;

class IDecodeStateCb {
public:
    IDecodeStateCb();
    virtual void DecodePrepare(IDecoder *decoder) = 0;
    virtual void DecodeReady(IDecoder *decoder) = 0;
    virtual void DecodeRunning(IDecoder *decoder) = 0;
    virtual void DecodePause(IDecoder *decoder) = 0;
    virtual bool DecodeOneFrame(IDecoder *decoder, OneFrame *frame) = 0;
    virtual void DecodeFinish(IDecoder *decoder) = 0;
    virtual void DecodeStop(IDecoder *decoder) = 0;
};

#endif //LEARNVIDEO_DECODE_STATE_CB_H
