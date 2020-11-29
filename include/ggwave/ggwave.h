#pragma once

#include <array>
#include <complex>
#include <cstdint>
#include <functional>

namespace RS {
class ReedSolomon;
}

class GGWave {
public:
    enum TxMode {
        FixedLength = 0,
        VariableLength,
    };

    static constexpr auto kMaxSamplesPerFrame = 1024;
    static constexpr auto kMaxDataBits = 256;
    static constexpr auto kMaxDataSize = 256;
    static constexpr auto kMaxLength = 140;
    static constexpr auto kMaxSpectrumHistory = 4;
    static constexpr auto kMaxRecordedFrames = 64*10;
    static constexpr auto kDefaultFixedLength = 82;

    using AmplitudeData   = std::array<float, kMaxSamplesPerFrame>;
    using AmplitudeData16 = std::array<int16_t, kMaxRecordedFrames*kMaxSamplesPerFrame>;
    using SpectrumData    = std::array<float, kMaxSamplesPerFrame>;
    using RecordedData    = std::array<float, kMaxRecordedFrames*kMaxSamplesPerFrame>;

    using CBQueueAudio = std::function<void(const void * data, uint32_t nBytes)>;
    using CBDequeueAudio = std::function<uint32_t(void * data, uint32_t nMaxBytes)>;

    GGWave(
            int aSampleRateIn,
            int aSampleRateOut,
            int aSamplesPerFrame,
            int aSampleSizeBytesIn,
            int aSampleSizeBytesOut);

    void setTxMode(TxMode aTxMode) { txMode = aTxMode; }

    bool setParameters(
            int aParamFreqDelta,
            int aParamFreqStart,
            int aParamFramesPerTx,
            int aParamBytesPerTx,
            int aParamVolume);

    bool init(int textLength, const char * stext);

    void send(const CBQueueAudio & cbQueueAudio);
    void receive(const CBDequeueAudio & CBDequeueAudio);

    const bool & getHasData() const { return hasData; }

    const int & getFramesToRecord()         const { return framesToRecord; }
    const int & getFramesLeftToRecord()     const { return framesLeftToRecord; }
    const int & getFramesToAnalyze()        const { return framesToAnalyze; }
    const int & getFramesLeftToAnalyze()    const { return framesLeftToAnalyze; }
    const int & getSamplesPerFrame()        const { return samplesPerFrame; }
    const int & getSampleSizeBytesIn()      const { return sampleSizeBytesIn; }
    const int & getSampleSizeBytesOut()     const { return sampleSizeBytesOut; }
    const int & getTotalBytesCaptured()     const { return totalBytesCaptured; }

    const float & getSampleRateIn()     const { return sampleRateIn; }
    const float & getAverageRxTime_ms() const { return averageRxTime_ms; }

    const std::array<std::uint8_t, kMaxDataSize> & getRxData() const { return rxData; }

private:
    int nIterations;

    int paramFreqDelta = 6;
    int paramFreqStart = 40;
    int paramFramesPerTx = 6;
    int paramBytesPerTx = 2;
    int paramECCBytesPerTx = 32; // used for fixed-length Tx
    int paramVolume = 10;

    // Rx
    bool receivingData;
    bool analyzingData;

    int nCalls = 0;
    int recvDuration_frames;
    int totalBytesCaptured;

    float tSum_ms = 0.0f;
    float averageRxTime_ms = 0.0;

    std::array<float, kMaxSamplesPerFrame> fftIn;
    std::array<std::complex<float>, kMaxSamplesPerFrame> fftOut;

    AmplitudeData sampleAmplitude;
    SpectrumData sampleSpectrum;

    std::array<std::uint8_t, kMaxDataSize> rxData;
    std::array<std::uint8_t, kMaxDataSize> txData;
    std::array<std::uint8_t, kMaxDataSize> txDataEncoded;

    int historyId = 0;
    AmplitudeData sampleAmplitudeAverage;
    std::array<AmplitudeData, kMaxSpectrumHistory> sampleAmplitudeHistory;

    RecordedData recordedAmplitude;

    // Tx
    bool hasData;

    float freqDelta_hz;
    float freqStart_hz;
    float hzPerFrame;
    float ihzPerFrame;
    float isamplesPerFrame;
    float sampleRateIn;
    float sampleRateOut;
    float sendVolume;

    int frameId;
    int framesLeftToAnalyze;
    int framesLeftToRecord;
    int framesPerTx;
    int framesToAnalyze;
    int framesToRecord;
    int freqDelta_bin = 1;
    int nBitsInMarker;
    int nDataBitsPerTx;
    int nECCBytesPerTx;
    int nMarkerFrames;
    int nPostMarkerFrames;
    int sampleSizeBytesIn;
    int sampleSizeBytesOut;
    int samplesPerFrame;
    int sendDataLength;

    std::string textToSend;

    TxMode txMode = TxMode::FixedLength;

    AmplitudeData outputBlock;
    AmplitudeData16 outputBlock16;

    std::array<bool, kMaxDataBits> dataBits;
    std::array<double, kMaxDataBits> phaseOffsets;
    std::array<double, kMaxDataBits> dataFreqs_hz;

    std::array<AmplitudeData, kMaxDataBits> bit1Amplitude;
    std::array<AmplitudeData, kMaxDataBits> bit0Amplitude;

    RS::ReedSolomon * rsData = nullptr;
    RS::ReedSolomon * rsLength = nullptr;
};