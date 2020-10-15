//
// 编解码常量定义
// Author: Chen Xiaoping
//

#ifndef LEARNVIDEO_ENCODE_CONST_H
#define LEARNVIDEO_ENCODE_CONST_H

extern "C" {
#include <libavutil/samplefmt.h>
};

// 音频编码格式：浮点型数据（32位）
static const AVSampleFormat ENCODE_AUDIO_DEST_FORMAT = AV_SAMPLE_FMT_FLTP;
// 音频编码采样率
static const int ENCODE_AUDIO_DEST_SAMPLE_RATE = 44100;
// 音频编码通道数
static const int ENCODE_AUDIO_DEST_CHANNEL_COUNTS = 2;
// 音频编码声道格式
static const uint64_t ENCODE_AUDIO_DEST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
// 音频编码比特率
static const int ENCODE_AUDIO_DEST_BIT_RATE = 64000;
// ACC音频一帧采样数
static const int ACC_NB_SAMPLES = 1024;
// 视频编码帧率
static const int ENCODE_VIDEO_FPS = 25;

#endif //LEARNVIDEO_ENCODE_CONST_H
