//
// OpenSL ES音频渲染器
// Author: Chen Xiaoping
// Create Date: 2019-08-02
//

#ifndef LEARNVIDEO_AUDIOPLAYER_H
#define LEARNVIDEO_AUDIOPLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <queue>
#include <string>
#include <pthread.h>
#include "../../../utils/logger.h"
#include "audio_render.h"

extern "C" {
#include <libavutil/mem.h>
};

static const char *TAG = "OpenSLRender";

class OpenSLRender: public AudioRender {

private:

    class PcmData {
    public:
        PcmData(uint8_t *pcm, int size) {
            this->pcm = pcm;
            this->size = size;
        }
        ~PcmData() {
            if (pcm != NULL) {
                //释放已使用的内存
                free(pcm);
                pcm = NULL;
                used = false;
            }
        }
        uint8_t *pcm = NULL;
        int size = 0;
        bool used = false;
    };

    const SLuint32 SL_QUEUE_BUFFER_COUNT = 2;

    // 引擎接口
    SLObjectItf m_engine_obj = NULL;
    SLEngineItf m_engine = NULL;

    //混音器
    SLObjectItf m_output_mix_obj = NULL;
    SLEnvironmentalReverbItf m_output_mix_evn_reverb = NULL;
    SLEnvironmentalReverbSettings m_reverb_settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;

    //pcm播放器
    SLObjectItf m_pcm_player_obj = NULL;
    SLPlayItf m_pcm_player = NULL;
    SLVolumeItf m_pcm_player_volume = NULL;

    //缓冲器队列接口
    SLAndroidSimpleBufferQueueItf m_pcm_buffer;

    std::queue<PcmData *> m_data_queue;

    // 缓存线程等待锁变量
    pthread_mutex_t m_cache_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t m_cache_cond = PTHREAD_COND_INITIALIZER;

    bool CreateEngine();
    bool CreateOutputMixer();
    bool ConfigPlayer();
    void StartRender();
    void BlockEnqueue();

    bool CheckError(SLresult result, std::string hint);

    void static sRenderPcm(OpenSLRender *that);
    void static sReadPcmBufferCbFun(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context);

    void WaitForCache() {
        pthread_mutex_lock(&m_cache_mutex);
        pthread_cond_wait(&m_cache_cond, &m_cache_mutex);
        pthread_mutex_unlock(&m_cache_mutex);
    }

    void SendCacheReadySignal() {
        pthread_mutex_lock(&m_cache_mutex);
        pthread_cond_signal(&m_cache_cond);
        pthread_mutex_unlock(&m_cache_mutex);
    }

public:
    OpenSLRender();
    ~OpenSLRender();

    void InitRender() override;
    void Render(uint8_t *pcm, int size) override;
    void ReleaseRender() override;
};


#endif //LEARNVIDEO_AUDIOPLAYER_H
