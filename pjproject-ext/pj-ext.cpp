#include <pjmedia/frame.h>
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
#include <pjsua2/types.hpp>
#include <pjsua2/media.hpp>
#include <string>
#include <pj/log.h>
#include <pjlib.h>
#include <pjlib-util.h>

#include "pj-ext.h"

#define THIS_FILE "library.cpp"

#define PJ2BOOL(var) ((var) != PJ_FALSE)

using std::string;

//static inline pj_str_t str2Pj(const string &input_str) {
//    pj_str_t output_str;
//    output_str.ptr = (char *) input_str.c_str();
//    output_str.slen = input_str.size();
//    return output_str;
//}
//
//static inline string pj2Str(const pj_str_t &input_str) {
//    if (input_str.ptr && input_str.slen > 0)
//        return string(input_str.ptr, input_str.slen);
//    return string();
//}

static inline int64_t high_resolution_now() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

PiAudioFrameBuffer::PiAudioFrameBuffer(void *encoder, int port, unsigned frames, unsigned samplesPerFrame) {
    _ring = std::vector<PiAudioFrame>(frames);
    _bufSize = samplesPerFrame * 2; // Only 16bit PCM is supported.
    _samplesPerFrame = samplesPerFrame;
    _count = 0;
    _size = 0;
    _head = 0;
    _tail = 0;

    for (auto &frame : _ring) {
        frame.encoder = encoder;
        frame.port = (uint16_t) port;
        frame.pcm_samples = samplesPerFrame;
    }
}

PiAudioFrameBuffer::~PiAudioFrameBuffer() {
}

PiAudioFrame *PiAudioFrameBuffer::operator[](size_t index) {
    if (index < 0) return nullptr;
    if (_size == 0) return nullptr;
    if (index >= _size) return nullptr;
    return &_ring[(_head + index) % _ring.size()];
}

int PiAudioFrameBuffer::push(uint32_t cycle, pjmedia_frame *frame, pj_uint64_t frameNum) {
    if (frame->size > PI_AUDIO_FRAME_MAX_PCM_BYTES) {
        return -1;
    }

    if (_size == 0) {
        _head = _count;
        _size++;
    } else {
        if (_size == _ring.size() - 1) {
            _head++;
        } else {
            _size++;
        }
    }

    auto newFrame = &_ring[_count % _ring.size()];
    newFrame->enqueuedAt = 0;
    newFrame->processedCPU = 0;
    newFrame->cycle = cycle;
    newFrame->timestamp = frame->timestamp.u64;
    newFrame->frame_num = frameNum;
    newFrame->vad_cpu = 0;
    newFrame->opus_cpu = 0;
    newFrame->processed = false;
    newFrame->dtx = false;
    newFrame->vad = 0;
    newFrame->pcm_bytes = (int16_t) frame->size;
    newFrame->pcm_samples = frame->size / 2;
    newFrame->opus_size = 0;

    // Copy PCM.
    memcpy((void *) &newFrame->pcm, frame->buf, frame->size);

    _tail = _count;
    _count++;
    return 0;
}

PiAudioFrame *PiAudioFrameBuffer::head() {
    if (_size == 0) return nullptr;
    return &_ring[_head % _ring.size()];
}

PiAudioFrame *PiAudioFrameBuffer::tail() {
    if (_size == 0) return nullptr;
    return &_ring[_tail % _ring.size()];
}

PiAudioFrame *PiAudioFrameBuffer::get(size_t index) {
    if (index < 0) return nullptr;
    if (_size == 0) return nullptr;
    if (index >= _size) return nullptr;
    return &_ring[(_head + index) % _ring.size()];
}

PiAudioFrame *PiAudioFrameBuffer::back(size_t count) {
    return get(_size - count - 1);
}


static const int PI_MAX_AUDIO_FRAMES = 4096;


//class PiAudioFrameNotifier {
//public:
//    PiAudioFrameNotifier() {
////        _thread = std::thread([this] { run(); });
//    }
//
//    ~PiAudioFrameNotifier() {
//        {
//            std::unique_lock<std::mutex> lock(_queueMutex);
//            _stop = true;
//        }
//        _thread.join();
//    }
//
//    static std::shared_ptr<PiAudioFrameNotifier> instance() {
//        static std::shared_ptr<PiAudioFrameNotifier> _notifier =
//                std::make_shared<PiAudioFrameNotifier>();
//        return _notifier;
//    }
//
//    void enqueue(PiAudioFrame *frame) {
//        std::unique_lock<std::mutex> lock(_queueMutex);
//        _queue.emplace(frame);
//    }
//
//    int poll(void **frames, int oldSize, int maxSize) {
//        // Release old frames.
//        if (oldSize > 0) {
//            auto f = frames;
//            for (int i = 0; i < oldSize; i++) {
//                auto encoder = ((PiAudioFrame *) (*f))->encoder;
//                if (encoder) {
//                    ((PiEncoder *) encoder)->onHeartbeat();
//                }
//                f++;
//            }
//        }
//
//        auto start = high_resolution_now();
//
//        std::unique_lock<std::mutex> lock(_queueMutex);
//
//        auto lockedAt = high_resolution_now();
//
//        // Notify.
//        _condition.wait(lock,
//                        [this] { return _stop || !_queue.empty(); });
//
//        auto notifiedAt = high_resolution_now();
//
//        // Add wait nanos.
//        _waitNanos += notifiedAt - lockedAt;
//        // Increment count.
//        _count++;
//
//        if (_stop && _queue.empty()) {
//            return 0;
//        }
//
//        int size = 0;
//        while (size <= PI_MAX_AUDIO_FRAMES && !_queue.empty()) {
//            auto msg = _queue.front();
//            *frames = (void *) msg;
//            frames++;
//            size++;
//            _queue.pop();
//        }
//
//        _lastPoll = high_resolution_now();
//        return size;
//    }
//
////    void setHandler(PiAudioFrameHandler *handler) {
////        std::unique_lock<std::mutex> lock(_queueMutex);
////        _handler = handler;
////    }
//
//private:
//    void onFrames(void **frames, int16_t size) {
////        if (_handler) {
////            _handler->onFrames(frames, size);
////        }
//    }
//
////    void run() {
////        std::vector<PiAudioFrame *> framesVec(PI_MAX_AUDIO_FRAMES);
////        framesVec.clear();
////        PiAudioFrameHandler *handler;
////
////        for (;;) {
////            {
////                int16_t size = 0;
////
////                auto start = high_resolution_now();
////                {
////                    std::unique_lock<std::mutex> lock(_queueMutex);
////                    _condition.wait(lock,
////                                    [this] { return _stop || !_queue.empty(); });
////
////                    auto end = high_resolution_now();
////                    auto waitedFor = start - end;
////
////                    if (_stop && _queue.empty())
////                        return;
////
////                    handler = _handler;
////
////                    while (size <= PI_MAX_AUDIO_FRAMES && !_queue.empty()) {
////                        size++;
////                        auto msg = _queue.front();
////                        framesVec.push_back(msg);
////                        _queue.pop();
////                    }
////                }
////
////                if (handler) {
////                    start = high_resolution_now();
////                    try {
////                        handler->onFrames((void **) framesVec.data(), size);
////                    } catch (...) {
////                        // Ignore.
////                    }
////                    auto end = high_resolution_now();
////                }
////
////                for (auto &f : framesVec) {
////                    f->processed = true;
////                }
////
////                std::unique_lock<std::mutex> lock(_queueMutex);
////
////                // Notify.
////                for (auto &msg : framesVec) {
////                    if (msg->encoder) {
////                        ((PiEncoder *) msg->encoder)->isSilent();
////                    }
////                }
////
////                framesVec.clear();
////            }
////        }
////    }
//
//private:
//    std::thread _thread;
//    uint64_t _count;
//    uint64_t _lastPoll;
//    uint64_t _waitNanos;
//    std::mutex _queueMutex;
//    std::condition_variable _condition;
//    std::queue<PiAudioFrame *> _queue;
//    bool _stop;
////    PiAudioFrameHandler *_handler;
//};

/**
 * ThreadPool
 */
class PiWorkerPool {
public:
    PiWorkerPool() : _stop(false) {
        addWorkers(1);
    }

    ~PiWorkerPool() {
        {
            std::unique_lock<std::mutex> lock(_queueMutex);
            _stop = true;
        }
        _condition.notify_all();
        for (std::thread &worker: _workers)
            worker.join();
    }

    void addWorkers(int count) {
        std::unique_lock<std::mutex> lock(_queueMutex);
        _workerCnt += count;
        for (auto i = 0; i < count; i++) {
            auto num = _workers.size();
            _workers.emplace_back(
                    [this, num] {
                        // Set thread name.
                        auto name = "pi-worker-" + std::to_string((int) num);
#if __APPLE__
                        pthread_setname_np(name.data());
#elif __linux__
                        pthread_setname_np(pthread_self(), name.data());
#elif __unix__
                        pthread_setname_np(name.data());
#endif

                        // Register with PJSIP.
                        Endpoint::instance().libRegisterThread(name);

                        // Run task loop.
                        for (;;) {
                            std::function<void()> task;

                            {
                                std::unique_lock<std::mutex> lock(_queueMutex);
                                this->_condition.wait(lock,
                                                      [this] { return _stop || !_tasks.empty(); });
                                if (_stop && _tasks.empty())
                                    return;
                                task = std::move(_tasks.front());
                                _tasks.pop();
                            }

                            // Run task.
                            task();
                        }
                    }
            );
        }
    }

    void stop() {
        std::unique_lock<std::mutex> lock(_queueMutex);
        _stop = true;
        _condition.notify_all();
    }

    template<typename F, typename... Args>
    auto enqueue(F &&f, Args &&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(_queueMutex);

            // don't allow enqueueing after stopping the pool
            if (_stop)
                throw std::runtime_error("enqueue on stopped TaskPool");

            _tasks.emplace([task]() { (*task)(); });
        }
        _condition.notify_one();
        return res;
    }

    inline static int numWorkers() {
        return get()->_workerCnt;
    }

    inline static std::shared_ptr<PiWorkerPool> get() {
        return instance();
    }

public:
    static std::shared_ptr<PiWorkerPool> instance() {
        static std::shared_ptr<PiWorkerPool> _workers =
                std::make_shared<PiWorkerPool>();
        return _workers;
    }

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> _workers;
    int _workerCnt;
    // the task queue
    std::queue<std::function<void()>> _tasks;

    // synchronization
    std::mutex _queueMutex;
    std::condition_variable _condition;
    bool _stop;
};

#define PJMEDIA_SIG_PORT_PI_RECORDER_PORT    PJMEDIA_SIG_CLASS_PORT_AUD('X','R')
#define PJMEDIA_SIG_PORT_PI_PORT    PJMEDIA_SIG_CLASS_PORT_AUD('X','P')

static const int PI_ENCODER_LOOKBACK = 40;

PiRecorder::PiRecorder() : AudioMedia() {
}

PiRecorder::~PiRecorder() {
    std::unique_lock<std::mutex> lock(_mutex);

    unregisterMediaPort();

    if (_vad) {
        fvad_free(_vad);
        _vad = nullptr;
    }
    if (_encoder) {
        opus_encoder_destroy(_encoder);
        _encoder = nullptr;
    }

    _frames->clear();
}

int PiRecorder::setVadMode(int mode) {
    std::unique_lock<std::mutex> lock(_mutex);
    if (_vad != nullptr) {
        return fvad_set_mode(_vad, mode);
    }
    return 0;
}


void PiRecorder::create() PJSUA2_THROW(Error) {

    pjsua_logging_config_default(&pjsua_var.log_cfg);
    std::unique_lock<std::mutex> lock(_mutex);

    if (id != PJSUA_INVALID_ID) {
        PJSUA2_RAISE_ERROR(PJ_EEXISTS);
    }

    pj_status_t status;

    status = pjsua_conf_get_port_info(0, &_masterInfo);
    PJSUA2_CHECK_RAISE_ERROR(status);

    pj_str_t name = pj_str((char *) "pi_recorder");
    status = pjmedia_port_info_init(
            &_base.info,
            &name,
            PJMEDIA_SIG_PORT_PI_RECORDER_PORT,
            _masterInfo.clock_rate,
            _masterInfo.channel_count,
            _masterInfo.bits_per_sample,
            _masterInfo.samples_per_frame
    );
    PJSUA2_CHECK_RAISE_ERROR(status);

    _ptime = _base.info.fmt.det.aud.frame_time_usec / 1000;
    _base.put_frame = &PiRecorder::on_put_frame;
    _base.get_frame = &PiRecorder::on_get_frame;
    _base.on_destroy = &PiRecorder::on_destroy;
    _base.port_data.pdata = this;
    _base.port_data.ldata = sizeof(PiRecorder);

    // Setup the PCM frame ring.
    auto historyFrames = 10;// 1000 / mPtime; // Keep 1 second of history.
    if (_ptime >= PI_ENCODER_LOOKBACK) {
        _dtxRewind = 1;
    } else {
        _dtxRewind = PI_ENCODER_LOOKBACK / _ptime;
    }

    // Setup OpusEncoder.
    int err;
    _encoder = opus_encoder_create((int) _masterInfo.clock_rate, 1, OPUS_APPLICATION_VOIP, &err);
    opus_encoder_ctl(_encoder, OPUS_SET_DTX(true));
//    opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(_masterInfo.clock_rate));
    opus_encoder_ctl(_encoder, OPUS_SET_BITRATE(20000));
//    opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(1));
//    opus_encoder_ctl(encoder, OPUS_SET_PACKET_LOSS_PERC(1));
    opus_encoder_ctl(_encoder, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_MEDIUMBAND));

    // Setup VAD.
    _vad = fvad_new();
    fvad_set_sample_rate(_vad, (int) _masterInfo.clock_rate);
    fvad_set_mode(_vad, _vadMode);

    PJSUA2_CHECK_RAISE_ERROR(status);

    registerMediaPort(&_base);

    _frames = std::make_unique<PiAudioFrameBuffer>(
            (void *) this,
            (uint16_t) id,
            historyFrames,
            _masterInfo.samples_per_frame);
}

PiEncoderStats PiRecorder::reset() {
    std::unique_lock<std::mutex> lock(_mutex);
    if (_isEncoding) {
        auto start = std::chrono::high_resolution_clock::now();
        _condition.wait(lock, [this] { return !_isEncoding; });
        auto end = std::chrono::high_resolution_clock::now() - start;
        _stats.totalEncoderWaitNanos += end.count();
    }

    auto old_stats = _stats;
    _stats = PiEncoderStats();

    if (_encoder != nullptr) {
        auto err = opus_encoder_init(_encoder, _masterInfo.clock_rate, 1, OPUS_APPLICATION_VOIP);
        if (err) {

        }
//        err = opus_encoder_ctl(mEncoder, OPUS_SET_INBAND_FEC(fec));
//        if (packetLossPct > -1) {
//            err = opus_encoder_ctl(mEncoder, OPUS_SET_PACKET_LOSS_PERC(fec));
//        }
    }
    _vadState = 0;
    if (_vad != nullptr) {
        fvad_reset(_vad);
    }

    _frames->clear();

    // Clear DTX.
    _inDtx = false;
    _cycle++;

    return old_stats;
}

pj_status_t PiRecorder::on_put_frame(pjmedia_port *this_port, pjmedia_frame *frame) {
    auto port = (PiRecorder *) (this_port->port_data.pdata);
    port->doPutFrame(frame);
    return PJ_SUCCESS;
}

pj_status_t PiRecorder::on_get_frame(pjmedia_port *this_port, pjmedia_frame *frame) {
    return PJ_SUCCESS;
}

pj_status_t PiRecorder::on_destroy(pjmedia_port *this_port) {
    ((PiRecorder *) (this_port->port_data.pdata))->onDestroy();
    return PJ_SUCCESS;
}

inline void PiRecorder::doPutFrame(pjmedia_frame *frame) {
    // Treat NONE frame types as a heartbeat.
    if (frame->type == PJMEDIA_FRAME_TYPE_NONE) {
        std::unique_lock<std::mutex> lock(_mutex);
        auto start = high_resolution_now();
        onHeartbeat();
        _stats.heartbeatCount++;
        _stats.totalHeartbeatCpu += (high_resolution_now() - start);
        return;
    }

    // Obtain lock.
    std::unique_lock<std::mutex> lock(_mutex);
    if (_isEncoding) {
        auto start = high_resolution_now();
        _condition.wait(lock, [this] { return !_isEncoding; });
        _stats.totalEncoderWaitNanos += high_resolution_now() - start;
    }
    _isEncoding = true;
    _enqueuedAt = high_resolution_now();

    try {
        // Ensure the PCM buffer has memory.
        if (_frames->bufferSize() != frame->size) {
            throw Error(
                    PJ_ERRNO_START,
                    "PiEncoder::doPutFrame",
                    "frame numWorkers changed",
                    "media.cpp",
                    384
            );
        }

        // Push the next frame.
        _frames->push(_cycle, frame, _stats.frameCount);

        // Enqueue.
        PiWorkerPool::get()->enqueue([this] { workerRun(); });
    } catch (Error &e) {
        this->onError(e);
    }
}

inline void PiRecorder::workerEncode(PiAudioFrame *frame) {
    auto start = high_resolution_now();
    // Encode Opus.
    frame->opus_size = opus_encode(
            _encoder,
            (opus_int16 *) &frame->pcm,
            (int) frame->pcm_samples,
            (unsigned char *) &frame->opus,
            PI_AUDIO_FRAME_MAX_OPUS_BYTES
    );
    auto externalStart = high_resolution_now();
    frame->opus_cpu = (externalStart - start);

//    int dtxState;
//    auto err = opus_encoder_ctl(mEncoder, OPUS_GET_IN_DTX(&dtxState));

    frame->processed = true;
    frame->dtx = false;

    // Call into
    onFrame(
            frame,
            _stats.lastExternCpu
    );

    _stats.totalOpusCpu += frame->opus_cpu;
    _stats.lastExternCpu = (pj_uint64_t) (high_resolution_now() - externalStart);
    _stats.totalExternCpu += _stats.lastExternCpu;
}

inline void PiRecorder::workerEncodeDTX(PiAudioFrame *frame) {
    auto start = high_resolution_now();
    frame->processed = true;
    frame->dtx = true;

    // Call into
    onFrame(
            frame,
            _stats.lastExternCpu
    );

    _stats.lastExternCpu = (pj_uint64_t) (high_resolution_now() - start);
    _stats.totalExternCpu += _stats.lastExternCpu;
}

inline void PiRecorder::workerDTX(PiAudioFrame *frame) {
    auto start = high_resolution_now();

    // Call into
//    onFrameDTX(
//            mCycle,
//            frame,
//            mStats.lastExternCpu
//    );

    _stats.lastExternCpu = (pj_uint64_t) (high_resolution_now() - start);
    _stats.totalExternCpu += _stats.lastExternCpu;
    _stats.frameCount++;
    _totalFrames++;
}

/**
 * Invoked on a worker thread.
 */
inline void PiRecorder::workerRun() {
    auto start = high_resolution_now();
    auto enqueue_end = start - _enqueuedAt;
    std::unique_lock<std::mutex> lock(_mutex);
    auto vad_start = high_resolution_now();

    _stats.totalEnqueueNanos += enqueue_end;
    auto frame = _frames->tail();

    // Process VAD.
    _vadState = fvad_process(_vad, (int16_t *) &frame->pcm, (size_t) frame->pcm_samples);
    auto end_vad = high_resolution_now();

    frame->vad = _vadState;

    // Calculate VAD cpu time.
    frame->vad_cpu = (end_vad - vad_start);
    _stats.totalVadCpu += frame->vad_cpu;

//    if (_dtx) {}
//    Silence?
//    if (!_vadState) {
//        if (!_inDtx) {
//            // Encode frame.
//            workerEncode(frame);
//
//            _stats.frameCount++;
//            _totalFrames++;
//            _inDtx = true;
//        } else {
//            frame->dtx = true;
//
//            // Skip OPUS.
//            _stats.dtxFramesSkipped++;
//
//            // Flush delayed frame.
//            auto flushFrame = _frames->back(_dtxRewind);
//            if (flushFrame && !flushFrame->processed) {
//                workerEncodeDTX(flushFrame);
//            }
//
//            // Invoke latest DTX
////            encode(frame);
//            workerDTX(frame);
//        }
//    } else {
//        if (_inDtx) {
//            _inDtx = false;
//
//            // VAD signal lags a little behind.
//            for (int i = _dtxRewind; i > 0; i--) {
//                auto f = _frames->back(i);
//                if (!f || f->processed) continue;
//                _stats.dtxFramesSkipped--;
//                workerEncode(f);
//
////                if (mOpusResult == 1) {
////                    mStats.dtxFramesMissed++;
////                }
//            }
//        }
//
//        workerEncode(frame);
//        _stats.frameCount++;
//        _totalFrames++;
//    }
    workerEncode(frame);
    _stats.frameCount++;
    _totalFrames++;

    // Flip encoding flag.
    _isEncoding = false;
    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    lock.unlock();

    _condition.notify_one();
}

int PiRecorder::encoderThreads() {
    return PiWorkerPool::get()->numWorkers();
}

void PiRecorder::addEncoderThreads(int count) {
    PiWorkerPool::get()->addWorkers(count);
}


PiPort::PiPort() {

}

PiPort::~PiPort() {
    if (id != PJSUA_INVALID_ID) {
        unregisterMediaPort();
        // Destroy.
        pjmedia_port_destroy(&_base);
    }
}

void PiPort::create() PJSUA2_THROW(Error) {
    if (id != PJSUA_INVALID_ID) {
        PJSUA2_RAISE_ERROR(PJ_EEXISTS);
    }

    pj_status_t status;

    status = pjsua_conf_get_port_info(0, &_masterInfo);
    PJSUA2_CHECK_RAISE_ERROR(status);

    pj_str_t name = pj_str((char *) "pi_port");
    status = pjmedia_port_info_init(
            &_base.info,
            &name,
            PJMEDIA_SIG_PORT_PI_PORT,
            _masterInfo.clock_rate,
            _masterInfo.channel_count,
            _masterInfo.bits_per_sample,
            _masterInfo.samples_per_frame
    );
    PJSUA2_CHECK_RAISE_ERROR(status);

    _ptime = _base.info.fmt.det.aud.frame_time_usec / 1000;
    _base.put_frame = &PiPort::on_put_frame;
    _base.get_frame = &PiPort::on_get_frame;
    _base.on_destroy = &PiPort::on_destroy;
    _base.port_data.pdata = this;
    _base.port_data.ldata = sizeof(PiPort);

    registerMediaPort(&_base);
}

unsigned PiPort::getClockRate() { return _masterInfo.clock_rate; }

unsigned PiPort::getChannelCount() { return _masterInfo.clock_rate; }

unsigned PiPort::getSamplesPerFrame() { return _masterInfo.samples_per_frame; }

unsigned PiPort::getBitsPerSample() { return _masterInfo.bits_per_sample; }

unsigned PiPort::getPtime() { return _ptime; }

float PiPort::getTxLevelAdj() { return _masterInfo.tx_level_adj; }

float PiPort::getRxLevelAdj() { return _masterInfo.rx_level_adj; }

pj_status_t PiPort::on_put_frame(pjmedia_port *this_port, pjmedia_frame *frame) {
    auto port = (PiPort *) (this_port->port_data.pdata);
    port->onPutFrame(frame->type, frame->buf, frame->size, frame->timestamp.u64, frame->bit_info);
    return PJ_SUCCESS;
}

pj_status_t PiPort::on_get_frame(pjmedia_port *this_port, pjmedia_frame *frame) {
    auto port = (PiPort *) (this_port->port_data.pdata);
    port->onGetFrame(frame->type, frame->buf, frame->size, frame->timestamp.u64, frame->bit_info);
    return PJ_SUCCESS;
}

pj_status_t PiPort::on_destroy(pjmedia_port *this_port) {
    auto port = (PiPort *) (this_port->port_data.pdata);
    port->onDestroy();
    return PJ_SUCCESS;
}

static LogWriter *_logWriter = nullptr;

static void logFunc(int level, const char *data, int len) {
    if (!_logWriter) return;

    LogEntry entry;
    entry.level = level;
    entry.msg = string(data, len);
    entry.threadId = (long) (size_t) pj_thread_this();
    entry.threadName = string(pj_thread_get_name(pj_thread_this()));

    _logWriter->write(entry);
}


void PiConfigureLogging(LogConfig *logConfig) {
    _logWriter = logConfig->writer;

    pjsua_logging_config lc;
    lc.msg_logging = logConfig->msgLogging;
    lc.level = logConfig->level;
    lc.console_level = logConfig->consoleLevel;
    lc.decor = logConfig->decor;
    lc.log_file_flags = logConfig->fileFlags;
    lc.cb = &logFunc;
    pjsua_var.log_cfg = lc;

    if (logConfig->writer) {
        pj_log_set_log_func(&logFunc);
    }
}

int SizeofPiAudioFrame() {
    return sizeof(PiAudioFrame);
}

