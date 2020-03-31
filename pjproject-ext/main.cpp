#include <pjsua2.hpp>
#include <pjsua2/types.hpp>
#include <pjsua2/json.hpp>
#include <pjsua.h>
#include <iostream>
#include <pj/types.h>
#include <thread>

#include "pj-ext.h"

using namespace pj;
using namespace std;

#define THIS_FILE        "main.cpp"

class Pi2Port : public PiRecorder {
public:
    inline void onFrameDTX(void *_frame, pj_uint64_t prevExternCPU) override {
        auto frame = (PiAudioFrame*)_frame;
        total_extern_cpu_ += prevExternCPU;
        total_extern_count_++;
    }

    inline void onFrame(void *_frame, pj_uint64_t prevExternCPU) override {
        auto frame = (PiAudioFrame*)_frame;

        if (frame->frame_num != next_frame_) {
            cout << endl;
        }
        next_frame_ = frame->frame_num + 1;

        total_extern_count_++;
        size_per_sec_ += frame->pcm_samples;
        vad_cpu_per_sec_ += frame->vad_cpu;
        opus_cpu_per_sec_ += frame->opus_cpu;
        if (frame->opus_size > 0) {
            opus_size_per_sec_ += frame->opus_size;
        }
        if (frame->opus_cpu > 0) {
            opus_count++;
            opus_total += frame->opus_cpu;
        }
        if (frame->frame_num > 0 && frame->frame_num % 50 == 0) {
            auto workerCnt = 10;//PiWorkerPool::numWorkers();
            if (vad_cpu_per_sec_ == 0) vad_cpu_per_sec_ = 1;
            if (opus_cpu_per_sec_ == 0) opus_cpu_per_sec_ = 1;
//            auto str = pj_thread_get_name(pj_thread_this());

            auto max_vad_per_sec = (1000000000 / vad_cpu_per_sec_) * (pj_uint64_t) workerCnt;
            auto max_opus_per_sec = (1000000000 / opus_cpu_per_sec_) * (pj_uint64_t) workerCnt;
            if (opus_cpu_per_sec_ == 1) {
                max_opus_per_sec = max_vad_per_sec;
            }
//            cout << str << " -> " << frameNum << "  size: " << size_per_sec << " opus size: "
//                 << opus_size_per_sec << "  VAD cpu:"
//                 << vad_cpu_per_sec
//                 << "  Opus cpu:"
//                 << opus_cpu_per_sec << endl;

            cout << "\t" << "Extern Count: " << total_extern_count_ << endl;
            cout << "\t" << "Extern CPU: " << total_extern_cpu_ << endl;
            cout << "\t" << "Extern Per Call: " << total_extern_cpu_ / total_extern_count_ << endl;

//            cout << "\t" << "wait for encoder nanos: " << getStats().totalEncoderWaitNanos << endl;
//            cout << "\t" << "  VAD per Sec:"
//                 << max_vad_per_sec
//                 << "  Opus per Sec:"
//                 << max_opus_per_sec << endl;

            opus_size_per_sec_ = 0;
            vad_cpu_per_sec_ = 0;
            opus_cpu_per_sec_ = 0;
            total_extern_cpu_ = 0;
            total_extern_count_ = 0;
        }

        total_extern_cpu_ += prevExternCPU;
    }

private:
    pj_uint64_t next_frame_;
    int size_per_sec_;
    int opus_size_per_sec_;
    pj_uint64_t vad_cpu_per_sec_;
    pj_uint64_t opus_cpu_per_sec_;
    int dtx_skipped;
    pj_uint64_t opus_count;
    pj_uint64_t opus_total;
    pj_uint64_t total_extern_cpu_;

    int total_extern_count_;
};



//class PiEndpoint : public Endpoint {
//
//};
//
//PiEndpoint *pidato_create_endpoint() {
//    auto ep = new PiEndpoint();
//    return ep;
//}
//
//void pidato_start_endpoint(PiEndpoint *ep) {
//    ep->libStart();
//}
//
//PiEncoder *pidato_create_port(int slot) {
//    auto port = new PiEncoder;
//    return port;
//}

int main() {
    cout << "struct PiAudioFrame: " << sizeof(PiAudioFrame) << endl;

    Endpoint ep;
    ep.libCreate();

    // Initialize endpoint
    EpConfig ep_cfg;
    ep_cfg.logConfig.level = 1;
    ep_cfg.medConfig.maxMediaPorts = 2048;
    ep_cfg.uaConfig.maxCalls = 256;
    ep_cfg.medConfig.audioFramePtime = 20;
    ep_cfg.medConfig.ptime = 20;
//    ep_cfg.medConfig.sndRecLatency = 20;
    ep_cfg.medConfig.clockRate = 16000;
    ep_cfg.medConfig.sndClockRate = 16000;
    ep_cfg.medConfig.ecOptions =
            PJMEDIA_ECHO_WEBRTC | PJMEDIA_ECHO_USE_NOISE_SUPPRESSOR | PJMEDIA_ECHO_AGGRESSIVENESS_AGGRESSIVE;
    ep_cfg.medConfig.ecTailLen = 100;
    ep_cfg.medConfig.hasIoqueue = true;
    ep_cfg.medConfig.threadCnt = 1;
    ep_cfg.medConfig.channelCount = 1;
    ep_cfg.medConfig.quality = 10;

    ep.libInit(ep_cfg);

    // Create SIP transport. Error handling sample is shown
    TransportConfig tcfg;
    tcfg.port = 5070;
    try {
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    } catch (Error &err) {
        std::cout << err.info() << std::endl;
        return 1;
    }

    // Start the library (worker threads etc)
    ep.libStart();

    std::cout << "Max Calls: " << ep_cfg.uaConfig.maxCalls << std::endl;
    std::cout << "Max Ports: " << ep.mediaMaxPorts() << std::endl;

    ep.audDevManager().setNullDev();

    {
        auto amp = std::make_unique<AudioMediaPlayer>();
//    amp.createPlayer("recording.wav", PJMEDIA_FILE_NO_LOOP);
        amp->createPlayer("recording.wav");

        auto customPort = std::make_unique<Pi2Port>();
        customPort->create();


        amp->startTransmit(*customPort);
//    amp2.startTransmit(*customPort2);
//    amp3.startTransmit(*customPort2);

        std::cin.getline(nullptr, 0);
//        pj_thread_sleep(50000);

        std::cout << "Stopping transmission" << std::endl;
//    amp.stopTransmit(*customPort);
//
//    amp.setPos(0);
//    amp.startTransmit(*customPort);

//        pj_thread_sleep(50000);

        amp->stopTransmit(*customPort);
        auto stats = customPort->reset();

        std::cout << std::endl;
    }

    ep.libDestroy();

    return 0;
}
