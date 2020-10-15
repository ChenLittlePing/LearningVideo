//
// 编码回调接口定义
//


#ifndef LEARNVIDEO_ENCODE_STATE_CB_H
#define LEARNVIDEO_ENCODE_STATE_CB_H

#include "../one_frame.h"

class IEncodeStateCb {
public:
    virtual void EncodeStart() = 0;
    virtual void EncodeSend() = 0;
    virtual void EncodeFrame(void *data) = 0;
    virtual void EncodeProgress(long time) = 0;
    virtual void EncodeFinish() = 0;
};

#endif //LEARNVIDEO_ENCODE_STATE_CB_H
