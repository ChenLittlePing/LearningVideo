//
// OpenSL ES音频渲染器
// Author: Chen Xiaoping
// Create Date: 2019-08-02
//

#include <thread>
#include <unistd.h>
#include "opensl_render.h"
#include "../../../utils/logger.h"

OpenSLRender::OpenSLRender() {
}

OpenSLRender::~OpenSLRender() {
}

void OpenSLRender::InitRender() {
    if (!CreateEngine()) return;
    if (!CreateOutputMixer()) return;
    if (!ConfigPlayer()) return;

    std::thread t(sRenderPcm, this);
    t.detach();
}

void OpenSLRender::Render(uint8_t *pcm, int size) {
    if (m_pcm_player) {
        if (pcm != NULL && size > 0) {
            while (m_data_queue.size() >= 2) {
                SendCacheReadySignal();
                usleep(20000);
            }
            // 将数据复制一份，并压入队列
            uint8_t *data = (uint8_t *) malloc(size);
            memcpy(data, pcm, size);

            PcmData *pcmData = new PcmData(data, size);
            m_data_queue.push(pcmData);

            // 通知播放线程推出等待，恢复播放
            SendCacheReadySignal();
        }
    } else {
        free(pcm);
    }
}

void OpenSLRender::ReleaseRender() {
    //设置停止状态
    if (m_pcm_player) {
        (*m_pcm_player)->SetPlayState(m_pcm_player, SL_PLAYSTATE_STOPPED);
        m_pcm_player = NULL;
    }

    // 先通知回调接口结束，否则可能导致无法销毁：m_pcm_player_obj
    SendCacheReadySignal();

    //销毁播放器
    if (m_pcm_player_obj) {
        (*m_pcm_player_obj)->Destroy(m_pcm_player_obj);
        m_pcm_player_obj = NULL;
        m_pcm_buffer = NULL;
    }
    //销毁混音器
    if (m_output_mix_obj) {
        (*m_output_mix_obj)->Destroy(m_output_mix_obj);
        m_output_mix_obj = NULL;
    }
    //销毁引擎
    if (m_engine_obj) {
        (*m_engine_obj)->Destroy(m_engine_obj);
        m_engine_obj = NULL;
        m_engine = NULL;
    }
    //释放缓存数据
    for (int i = 0; i < m_data_queue.size(); ++i) {
        PcmData *pcm = m_data_queue.front();
        m_data_queue.pop();
        delete pcm;
    }
}

bool OpenSLRender::CreateEngine() {
    SLresult result = slCreateEngine(&m_engine_obj, 0, NULL, 0, NULL, NULL);
    if (CheckError(result, "Engine")) return false;

    result = (*m_engine_obj)->Realize(m_engine_obj, SL_BOOLEAN_FALSE);
    if (CheckError(result, "Engine Realize")) return false;

    result = (*m_engine_obj)->GetInterface(m_engine_obj, SL_IID_ENGINE, &m_engine);
    return !CheckError(result, "Engine Interface");
}

bool OpenSLRender::CreateOutputMixer() {
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    SLresult result = (*m_engine)->CreateOutputMix(m_engine, &m_output_mix_obj, 1, mids, mreq);
    if (CheckError(result, "Output Mix")) return false;

    result = (*m_output_mix_obj)->Realize(m_output_mix_obj, SL_BOOLEAN_FALSE);
    if (CheckError(result, "Output Mix Realize")) return false;

    result = (*m_output_mix_obj)->GetInterface(m_output_mix_obj, SL_IID_ENVIRONMENTALREVERB, &m_output_mix_evn_reverb);
    if (CheckError(result, "Output Mix Env Reverb")) return false;

    if (result == SL_RESULT_SUCCESS) {
         (*m_output_mix_evn_reverb)->SetEnvironmentalReverbProperties(m_output_mix_evn_reverb, &m_reverb_settings);
    }
    return true;
}

bool OpenSLRender::ConfigPlayer() {
    //配置PCM格式信息
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, SL_QUEUE_BUFFER_COUNT};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//播放pcm格式的数据
            (SLuint32)2,//2个声道（立体声）
            SL_SAMPLINGRATE_44_1,//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16,//和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//立体声（前左前右）
            SL_BYTEORDER_LITTLEENDIAN//结束标志
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    //配置音频池
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_output_mix_obj};
    SLDataSink slDataSink = {&outputMix, NULL};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult result = (*m_engine)->CreateAudioPlayer(m_engine, &m_pcm_player_obj, &slDataSource, &slDataSink, 3, ids, req);
    if (CheckError(result, "Player")) return false;

    //初始化播放器
    result = (*m_pcm_player_obj)->Realize(m_pcm_player_obj, SL_BOOLEAN_FALSE);
    if (CheckError(result, "Player Realize")) return false;

    //得到接口后调用，获取Player接口
    result = (*m_pcm_player_obj)->GetInterface(m_pcm_player_obj, SL_IID_PLAY, &m_pcm_player);
    if (CheckError(result, "Player Interface")) return false;

    //注册回调缓冲区，获取缓冲队列接口
    result = (*m_pcm_player_obj)->GetInterface(m_pcm_player_obj, SL_IID_BUFFERQUEUE, &m_pcm_buffer);
    if (CheckError(result, "Player Queue Buffer")) return false;

    //缓冲接口回调
    result = (*m_pcm_buffer)->RegisterCallback(m_pcm_buffer, sReadPcmBufferCbFun, this);
    if (CheckError(result, "Register Callback Interface")) return false;

    //获取音量接口
    result = (*m_pcm_player_obj)->GetInterface(m_pcm_player_obj, SL_IID_VOLUME, &m_pcm_player_volume);
    if (CheckError(result, "Player Volume Interface")) return false;

    LOGI(TAG, "OpenSL ES init success")

    return true;
}

bool OpenSLRender::CheckError(SLresult result, std::string hint) {
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "OpenSL ES [%s] init fail", hint.c_str())
        return true;
    }
    return false;
}

void OpenSLRender::sRenderPcm(OpenSLRender *that) {
    that->StartRender();
}

void OpenSLRender::StartRender() {
    while (m_data_queue.empty()) {
        WaitForCache();
    }
    (*m_pcm_player)->SetPlayState(m_pcm_player, SL_PLAYSTATE_PLAYING);
    sReadPcmBufferCbFun(m_pcm_buffer, this);
    LOGI(TAG, "openSL render start playing")
}

void OpenSLRender::sReadPcmBufferCbFun(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    OpenSLRender *player = (OpenSLRender *)context;
    player->BlockEnqueue();
}

void OpenSLRender::BlockEnqueue() {
    if (m_pcm_player == NULL) return;

    // 先将已经使用过的数据移除
    while (!m_data_queue.empty()) {
        PcmData *pcm = m_data_queue.front();
        if (pcm->used) {
            m_data_queue.pop();
            delete pcm;
        } else {
            break;
        }
    }

    // 等待数据缓冲
    while (m_data_queue.empty() && m_pcm_player != NULL) {// if m_pcm_player is NULL, stop render
        WaitForCache();
    }

    PcmData *pcmData = m_data_queue.front();
    if (NULL != pcmData && m_pcm_player) {
        SLresult result = (*m_pcm_buffer)->Enqueue(m_pcm_buffer, pcmData->pcm, (SLuint32) pcmData->size);
        if (result == SL_RESULT_SUCCESS) {
            // 只做已经使用标记，在下一帧数据压入前移除
            // 保证数据能正常使用，否则可能会出现破音
            pcmData->used = true;
        }
    }
}
