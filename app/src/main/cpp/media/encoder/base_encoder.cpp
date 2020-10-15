//
// 基础编码器
//

#include <unistd.h>
#include "base_encoder.h"

BaseEncoder::BaseEncoder(JNIEnv *env, Mp4Muxer *muxer, AVCodecID codec_id)
: m_muxer(muxer),
m_codec_id(codec_id) {
    if (Init()) {
        env->GetJavaVM(&m_jvm_for_thread);
        CreateEncodeThread();
    }
}

bool BaseEncoder::Init() {
    m_codec = avcodec_find_encoder(m_codec_id);
    if (m_codec == NULL) {
        LOGE(TAG, "Fail to find encoder, code id is %d", m_codec_id)
        return false;
    }
    m_codec_ctx = avcodec_alloc_context3(m_codec);
    if (m_codec_ctx == NULL) {
        LOGE(TAG, "Fail to alloc encoder context")
        return false;
    }

    m_encoded_pkt = av_packet_alloc();
    av_init_packet(m_encoded_pkt);

    return true;
}

void BaseEncoder::CreateEncodeThread() {
    // 使用智能指针，线程结束时，自动删除本类指针
    std::shared_ptr<BaseEncoder> that(this);
    std::thread t(Encode, that);
    t.detach();
}

static int encode_count = 0;
static int encode_in_count = 0;

void BaseEncoder::Encode(std::shared_ptr<BaseEncoder> that) {
    JNIEnv * env;

    //将线程附加到虚拟机，并获取env
    if (that->m_jvm_for_thread->AttachCurrentThread(&env, NULL) != JNI_OK) {
        LOG_ERROR(that->TAG, that->LogSpec(), "Fail to Init encode thread");
        return;
    }

    that->OpenEncoder();
    that->LoopEncode();
    that->DoRelease();
    //解除线程和jvm关联
    that->m_jvm_for_thread->DetachCurrentThread();

}

void BaseEncoder::OpenEncoder() {
    InitContext(m_codec_ctx);

    int ret = avcodec_open2(m_codec_ctx, m_codec, NULL);
    if (ret < 0) {
        LOG_ERROR(TAG, LogSpec(), "Fail to open encoder : %d", m_codec);
        return;
    }

    m_encode_stream_index = ConfigureMuxerStream(m_muxer, m_codec_ctx);
}

void BaseEncoder::LoopEncode() {
    if (m_state_cb != NULL) {
        m_state_cb->EncodeStart();
    }
    encode_count = 0;
    encode_in_count = 0;
    while (true) {
        if (m_src_frames.size() == 0) {
            Wait();
        }
        while (m_src_frames.size() > 0) {
            // 1. 获取待解码数据
            m_frames_lock.lock();
            OneFrame *one_frame = m_src_frames.front();
            m_src_frames.pop();
            m_frames_lock.unlock();
            encode_count++;

            AVFrame *frame = NULL;
            if (one_frame->line_size != 0) { //如果数据长度为0，说明编码已经结束，压入空frame，使编码器进入结束状态
                m_src_time_base = one_frame->time_base;
                // 2. 子类处理数据
                frame = DealFrame(one_frame);
                delete one_frame;
                if (m_state_cb != NULL) {
                    m_state_cb->EncodeSend();
                }
                if (frame == NULL) {
                    continue;
                }
            } else {
                delete one_frame;
            }
            // 3. 将数据发送到编码器
            int ret = avcodec_send_frame(m_codec_ctx, frame);
            switch (ret) {
                case AVERROR_EOF:
                    LOG_ERROR(TAG, LogSpec(), "Send frame finish [AVERROR_EOF]")
                    break;
                case AVERROR(EAGAIN): //编码编码器已满，先取出已编码数据，再尝试发送数据
                    while (ret == AVERROR(EAGAIN)) {
                        LOG_ERROR(TAG, LogSpec(), "Send frame error[EAGAIN]: %s", av_err2str(AVERROR(EAGAIN)));
                        // 4. 将编码好的数据榨干
                        if (DrainEncode()) return; //编码结束
                        // 5. 重新发送数据
                        ret = avcodec_send_frame(m_codec_ctx, frame);
                    }
                    break;
                case AVERROR(EINVAL):
                    LOG_ERROR(TAG, LogSpec(), "Send frame error[EINVAL]: %s", av_err2str(AVERROR(EINVAL)));
                    break;
                case AVERROR(ENOMEM):
                    LOG_ERROR(TAG, LogSpec(), "Send frame error[ENOMEM]: %s", av_err2str(AVERROR(ENOMEM)));
                    break;
                default:
//                    LOGE(TAG, "Send frame to encode, pts: %lld",
//                            (uint64_t )(one_frame.pts*av_q2d(one_frame.time_base)*1000))
                    break;
            }
            if (ret != 0) break;
        }

        if (DrainEncode()) break; //编码结束
    }
}

bool BaseEncoder::DrainEncode() {
    int state = EncodeOneFrame();
    while (state == 0) {
        state = EncodeOneFrame();
    }
    return state == AVERROR_EOF;
}

int BaseEncoder::EncodeOneFrame() {
    int state = avcodec_receive_packet(m_codec_ctx, m_encoded_pkt);
    switch (state) {
        case AVERROR_EOF: //解码结束
            LOG_ERROR(TAG, LogSpec(), "Encode finish")
            break;
        case AVERROR(EAGAIN): //编码还未完成，待会再来
            LOG_INFO(TAG, LogSpec(), "Encode error[EAGAIN]: %s", av_err2str(AVERROR(EAGAIN)));
            break;
        case AVERROR(EINVAL):
            LOG_ERROR(TAG, LogSpec(),  "Encode error[EINVAL]: %s", av_err2str(AVERROR(EINVAL)));
            break;
        case AVERROR(ENOMEM):
            LOG_ERROR(TAG, LogSpec(), "Encode error[ENOMEM]: %s", av_err2str(AVERROR(ENOMEM)));
            break;
        default:
            //将视频pts/dts转换为容器pts/dts，可以手动转换pts和dts，或直接使用av_packet_rescale_ts
//            m_encoded_pkt->pts = av_rescale_q(m_encoded_pkt->pts, m_src_time_base,
//                                              m_muxer->GetTimeBase(m_encode_stream_index));
//            m_encoded_pkt->dts = av_rescale_q(m_encoded_pkt->dts, m_src_time_base,
//                                              m_muxer->GetTimeBase(m_encode_stream_index));

            av_packet_rescale_ts(m_encoded_pkt, m_src_time_base,
                                 m_muxer->GetTimeBase(m_encode_stream_index));
            if (m_state_cb != NULL) {
                m_state_cb->EncodeFrame(m_encoded_pkt->data);
                long cur_time = (long)(m_encoded_pkt->pts*av_q2d(m_muxer->GetTimeBase(m_encode_stream_index))*1000);
                m_state_cb->EncodeProgress(cur_time);
            }
            m_encoded_pkt->stream_index = m_encode_stream_index;
            m_muxer->Write(m_encoded_pkt);
            break;
    }
    av_packet_unref(m_encoded_pkt);
    return state;
}

void BaseEncoder::PushFrame(OneFrame *one_frame) {
    m_frames_lock.lock();

    encode_in_count++;

    m_src_frames.push(one_frame);

    m_frames_lock.unlock();

    SendSignal();
}

void BaseEncoder::DoRelease() {
    if (m_encoded_pkt != NULL) {
        av_packet_free(&m_encoded_pkt);
        m_encoded_pkt = NULL;
    }
    if (m_codec_ctx != NULL) {
        avcodec_close(m_codec_ctx);
        avcodec_free_context(&m_codec_ctx);
    }
    Release();

    if (m_state_cb != NULL) {
        m_state_cb->EncodeFinish();
    }
    LOGE("cccccc", "all encode count : %d,  %d", encode_in_count, encode_count)
}