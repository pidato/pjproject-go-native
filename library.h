#ifndef PJPIDATO_LIBRARY_H
#define PJPIDATO_LIBRARY_H

#include <iostream>
#include <thread>
#include <chrono>
#include <future>
#include <vector>
#include <queue>
#include <functional>
#include <memory>

#include <pjsua2.hpp>
#include <pj/types.h>
#include <pjmedia/format.h>
#include <fvad.h>
#include <opus/opus.h>

using namespace std;
using namespace pj;

const int PI_AUDIO_FRAME_MAX_PCM_BYTES = 640;
const int PI_AUDIO_FRAME_MAX_OPUS_BYTES = 1328;



struct PiAudioFrame {
    void *encoder;
    uint64_t enqueuedAt;
    uint64_t processedCPU;
    uint16_t port;
    uint32_t cycle;
    uint64_t timestamp;
    int64_t frame_num;
    int64_t vad_cpu;
    int64_t opus_cpu;
    bool processed;
    bool dtx;
    int8_t vad;
    int16_t pcm_bytes;
    int16_t pcm_samples;
    int16_t opus_size;
    uint8_t pcm[PI_AUDIO_FRAME_MAX_PCM_BYTES];
    uint8_t opus[PI_AUDIO_FRAME_MAX_OPUS_BYTES];
};

/**
 *
 */
class PiAudioFrameBuffer {
public:
    PiAudioFrameBuffer(void *encoder, int port, unsigned frames, unsigned samplesPerFrame);

    ~PiAudioFrameBuffer();

    PiAudioFrame *operator[](int index);

    size_t bufferSize() { return _bufSize; }

    size_t samplesPerFrame() { return _samplesPerFrame; }

    int size() { return _size; }

    void clear() { _size = 0; }

    /**
     * memcpy the new tail position.
     *
     * @param buf must be the exact same numWorkers
     */
    int push(uint32_t cycle, pjmedia_frame *frame, pj_uint64_t frameNum);

    PiAudioFrame *head();

    PiAudioFrame *tail();

    PiAudioFrame *get(int index);

    PiAudioFrame *back(int count);

private:
    std::vector<PiAudioFrame> _ring;
    int64_t _bufSize;
    int64_t _samplesPerFrame;
    int64_t _count;
    int64_t _head;
    int64_t _tail;
    int _size;
};

/**
 *
 */
struct PiEncoderStats {
    pj_uint64_t frameCount;
    pj_uint64_t dtxFramesSkipped;
    pj_uint64_t dtxFramesMissed;
    pj_uint64_t totalVadCpu;
    pj_uint64_t totalOpusCpu;
    pj_uint64_t totalExternCpu;
    pj_uint64_t lastExternCpu;
    pj_uint64_t heartbeatCount;
    pj_uint64_t totalHeartbeatCpu;
    pj_uint64_t totalEnqueueNanos;
    pj_uint64_t totalEncoderWaitNanos;
};

//class PiAudioFrameHandler {
//public:
//    virtual void onFrames(void **frames, int16_t size) {
//
//    }
//};

/**
 * Encodes raw PCM to Opus with using WebRTC Voice Activity Detection to minimize
 * Opus encoder CPU cycles by a hybrid form of DTX (discontinuous transmission).
 */
class PiEncoder : public AudioMedia {
public:
    PiEncoder();

    virtual ~PiEncoder();

    /**
     *
     */
    void create() PJSUA2_THROW(Error);

    /**
     *
     * @return
     */
    unsigned getClockRate() { return _masterInfo.clock_rate; }

    /**
     *
     * @return
     */
    unsigned getChannelCount() { return _masterInfo.channel_count; }

    /**
     *
     * @return
     */
    unsigned getSamplesPerFrame() { return _masterInfo.samples_per_frame; }

    unsigned getPtime() { return _ptime; }

    unsigned getBitsPerSample() { return _masterInfo.bits_per_sample; }

    float getTxLevelAdj() { return _masterInfo.tx_level_adj; }

    float getRxLevelAdj() { return _masterInfo.rx_level_adj; }

    /**
     *
     */
    PiEncoderStats reset();

    /**
     *
     * @return
     */
    int getVadMode() { return _vadMode; }

    /**
     *
     * @param mode
     * @return
     */
    int setVadMode(int mode);

    /**
     * @return              : 1 - (active voice),
     *                        0 - (non-active Voice),
     *                       -1 - (invalid frame length).
     */
    int getVadState() { return _vadState; }

    bool isVadError() { return _vadState == -1; }

    bool isSpeaking() { return _vadState == 1; }

    bool isSilent() { return _vadState != 1; }

    /**
     * Returns a pointer to the OpusEncoder.
     *
     * @return
     */
    void *getOpusEncoder() { return _encoder; }

    PiEncoderStats getStats() { return _stats; }

    pj_uint64_t getTotalVadCpu() { return _stats.totalVadCpu; }

    pj_uint64_t getTotalOpusCpu() { return _stats.totalOpusCpu; }

    pj_uint64_t getTotalExternCpu() { return _stats.totalExternCpu; }

    pj_uint64_t getLastExternCpu() { return _stats.lastExternCpu; }

    virtual void onHeartbeat() {}

    virtual void onError(Error &e) {}

    /**
     * Invoked on each frame in DTX mode.
     *
     * @param cycle
     * @param frameNum
     * @param dtxLookback
     * @param vadState
     * @param pcm
     * @param size
     * @param timestamp
     * @param opusBuf
     * @param opusResult
     * @param vadCPU
     * @param opusCPU
     * @param prevExternCPU
     */
    virtual void onFrameDTX(
            void *frame,
            pj_uint64_t prevExternCPU
    ) {}

    /**
     *
     * @param frameNum
     * @param vad_state
     * @param pcm
     * @param size number of 16bit integers.
     * @param timestamp
     * @param bit_info
     * @param opusBuf
     * @param opusResult positive value is numWorkers in opus_buf; otherwise opus error code
     */
    virtual void onFrame(
            void *frame,
            pj_uint64_t prevExternCPU
    ) {}

    virtual void onDestroy() {}

    static int encoderThreads();

    static void addEncoderThreads(int count);

protected:

private:
    std::mutex _mutex;
    std::condition_variable _condition;

    bool _isEncoding;
    int64_t _enqueuedAt;

    pjmedia_port _base;
    pjsua_conf_port_info _masterInfo;
    unsigned _ptime;

    OpusEncoder *_encoder;
    Fvad *_vad;
    int _vadMode;
    int _vadState;
    uint32_t _cycle;

    bool _inDtx;
    int _dtxRewind; // Number of frames to rewind once VAD state moves to speech.
    std::unique_ptr<PiAudioFrameBuffer> _frames;
    PiEncoderStats _stats;

    uint64_t _totalFrames;

    void onProcessed(PiAudioFrame *frame);

    void doPutFrame(pjmedia_frame *frame);

    void workerEncode(PiAudioFrame *frame);

    void workerDTX(PiAudioFrame *frame);

    void workerEncodeDTX(PiAudioFrame *frame);

    void workerRun();

    static pj_status_t on_put_frame(pjmedia_port *this_port,
                                    pjmedia_frame *frame);

    static pj_status_t on_get_frame(pjmedia_port *this_port,
                                    pjmedia_frame *frame);

    static pj_status_t on_destroy(pjmedia_port *this_port);
};

/**
 * Minimal low level port port playing raw PCM data.
 */
class PiPlayer : public AudioMedia {
public:
    PiPlayer();

    virtual ~PiPlayer();

    void create() PJSUA2_THROW(Error);

    /**
     *
     * @return
     */
    unsigned getClockRate();

    /**
     *
     * @return
     */
    unsigned getChannelCount();

    /**
     *
     * @return
     */
    unsigned getSamplesPerFrame();

    /**
     * Frame duration in milliseconds.
     *
     * @return
     */
    unsigned getPtime();

    unsigned getBitsPerSample();

    float getTxLevelAdj();

    float getRxLevelAdj();

    /**
     * Called when the next frame is needed.
     *
     * @param frame_number
     * @param pcm pointer to buffer to copy data to
     * @param size maximum size of buffer
     * @param timestamp
     * @param bit_info
     */
    virtual void onGetFrame(
            void *pcm,
            pj_size_t size,
            pj_uint64_t timestamp,
            pj_uint32_t bit_info
    ) {}

private:
    pjmedia_port _base;
    pjsua_conf_port_info _masterInfo;
    unsigned ptime;

    static pj_status_t on_put_frame(pjmedia_port *this_port,
                                    pjmedia_frame *frame);

    static pj_status_t on_get_frame(pjmedia_port *this_port,
                                    pjmedia_frame *frame);

    static pj_status_t on_destroy(pjmedia_port *this_port);
};


#endif //PJPIDATO_LIBRARY_H
