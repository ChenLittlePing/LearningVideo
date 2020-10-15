//
// 编码器定义
//

#ifndef LEARNVIDEO_I_ENCODER_H
#define LEARNVIDEO_I_ENCODER_H

#include "../one_frame.h"
#include "i_encode_state_cb.h"

class IEncoder {
public:
    virtual void PushFrame(OneFrame *one_frame) = 0;
    virtual bool TooMuchData() = 0;
    virtual void SetStateReceiver(IEncodeStateCb *cb) = 0;
};
#endif //LEARNVIDEO_I_ENCODER_H
