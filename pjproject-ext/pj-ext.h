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

    PiAudioFrame *operator[](size_t index);

    size_t bufferSize() { return buf_size_; }

    size_t samplesPerFrame() { return samples_per_frame_; }

    int size() { return size_; }

    void clear() { size_ = 0; }

    /**
     * memcpy the new tail position.
     *
     * @param buf must be the exact same numWorkers
     */
    int push(uint32_t cycle, pjmedia_frame *frame, pj_uint64_t frameNum);

    PiAudioFrame *head();

    PiAudioFrame *tail();

    PiAudioFrame *get(size_t index);

    PiAudioFrame *back(size_t count);

private:
    std::vector<PiAudioFrame> _ring;
    long long buf_size_;
    long long samples_per_frame_;
    long long count_;
    long long head_;
    long long tail_;
    size_t size_;
};

/**
 *
 */
struct PiEncoderStats {
    unsigned long long frameCount;
    unsigned long long dtxFramesSkipped;
    unsigned long long dtxFramesMissed;
    unsigned long long totalVadCpu;
    unsigned long long totalOpusCpu;
    unsigned long long totalExternCpu;
    unsigned long long lastExternCpu;
    unsigned long long heartbeatCount;
    unsigned long long totalHeartbeatCpu;
    unsigned long long totalEnqueueNanos;
    unsigned long long totalEncoderWaitNanos;
};

/**
 * Encodes raw PCM to Opus with using WebRTC Voice Activity Detection to minimize
 * Opus encoder CPU cycles by a hybrid form of DTX (discontinuous transmission).
 */
class PiRecorder : public AudioMedia {
public:
    PiRecorder();

    virtual ~PiRecorder();

    /**
     *
     */
    void create() PJSUA2_THROW(Error);

    /**
     *
     * @return
     */
    unsigned getClockRate() { return master_info_.clock_rate; }

    /**
     *
     * @return
     */
    unsigned getChannelCount() { return master_info_.channel_count; }

    /**
     *
     * @return
     */
    unsigned getSamplesPerFrame() { return master_info_.samples_per_frame; }

    unsigned getPtime() { return ptime_; }

    unsigned getBitsPerSample() { return master_info_.bits_per_sample; }

    float getTxLevelAdj() { return master_info_.tx_level_adj; }

    float getRxLevelAdj() { return master_info_.rx_level_adj; }

    /**
     *
     */
    PiEncoderStats reset();

    /**
     *
     * @return
     */
    int getVadMode() { return vad_mode_; }

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
    int getVadState() { return vad_state_; }

    bool isVadError() { return vad_state_ == -1; }

    bool isSpeaking() { return vad_state_ == 1; }

    bool isSilent() { return vad_state_ != 1; }

    /**
     * Returns a pointer to the OpusEncoder.
     *
     * @return
     */
    void *getOpusEncoder() { return encoder_; }

    PiEncoderStats getStats() { return stats_; }

    unsigned long long getTotalVadCpu() { return stats_.totalVadCpu; }

    unsigned long long getTotalOpusCpu() { return stats_.totalOpusCpu; }

    unsigned long long getTotalExternCpu() { return stats_.totalExternCpu; }

    unsigned long long getLastExternCpu() { return stats_.lastExternCpu; }

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
    std::mutex mutex_;
    std::condition_variable condition_;

    bool is_encoding_;
    long long enqueued_at_;

    pjmedia_port base_;
    pjsua_conf_port_info master_info_;
    unsigned ptime_;

    OpusEncoder *encoder_;
    Fvad *vad_;
    int vad_mode_;
    int vad_state_;
    unsigned int cycle_;

    bool in_dtx_;
    int dtx_rewind_; // Number of frames to rewind once VAD state moves to speech.
    std::unique_ptr<PiAudioFrameBuffer> frames_;
    PiEncoderStats stats_;

    unsigned long long total_frames_;

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
class PiPort : public AudioMedia {
public:
    PiPort();

    virtual ~PiPort();

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
    virtual void onPutFrame(
            pjmedia_frame_type frameType,
            void *pcm,
            size_t size,
            unsigned long long timestamp,
            unsigned int bit_info
    ) {}

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
            pjmedia_frame_type frameType,
            void *pcm,
            size_t size,
            unsigned long long timestamp,
            unsigned int bit_info
    ) {}

    virtual void onDestroy() {}

private:
    pjmedia_port _base;
    pjsua_conf_port_info _masterInfo;
    unsigned _ptime;

    static pj_status_t on_put_frame(pjmedia_port *this_port,
                                    pjmedia_frame *frame);

    static pj_status_t on_get_frame(pjmedia_port *this_port,
                                    pjmedia_frame *frame);

    static pj_status_t on_destroy(pjmedia_port *this_port);
};

int SizeofPiAudioFrame();

void PiConfigureLogging(LogConfig *logConfig);


#endif //PJPIDATO_LIBRARY_H
