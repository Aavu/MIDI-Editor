//
// Created by Raghavasimhan Sankaranarayanan on 4/18/20.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class AudioExportComponent: public Component {
public:
    explicit AudioExportComponent(double sampleRate, int channels, int bitDepth) :
    m_bInitialized(false),
    m_fSampleRate(sampleRate),
    m_iNumChannels(channels),
    m_iBitDepth(bitDepth)
    {

    }

    ~AudioExportComponent() override {
        finish();
    }

    void startThread() {
        m_backgroundThread.startThread();
    }

    void init(const File& file) {
        if (m_bInitialized)
            return;
        if (auto fileStream = std::unique_ptr<FileOutputStream> (file.createOutputStream())) {
            WavAudioFormat wavFormat;
            if (auto writer = wavFormat.createWriterFor(fileStream.get(), m_fSampleRate, m_iNumChannels, m_iBitDepth, {}, 0)) {
                fileStream.release();
                m_threadedWriter = std::make_unique<AudioFormatWriter::ThreadedWriter>(writer, m_backgroundThread, 32768);

                const ScopedLock sl (m_writerLock);
                m_activeWriter = m_threadedWriter.get();
                if (m_backgroundThread.isThreadRunning())
                    m_bInitialized = true;
            }
        }
    }

    void write(const AudioSourceChannelInfo& bufferToFill) {
        if (!m_bInitialized)
            return;
        const ScopedLock sl (m_writerLock);
        if (m_activeWriter.load() != nullptr) // && m_backgroundThread.isThreadRunning())
        {
            m_activeWriter.load()->write (bufferToFill.buffer->getArrayOfReadPointers(), bufferToFill.numSamples);
        }
    }

    void finish() {
        {
            const ScopedLock sl (m_writerLock);
            m_activeWriter = nullptr;
        }

        m_threadedWriter.reset();
        m_backgroundThread.stopThread(1000);
        m_bInitialized = false;
    }

    bool isExporting() const
    {
        return m_activeWriter.load() != nullptr;
    }

private:
    bool m_bInitialized = false;

    TimeSliceThread m_backgroundThread { "Audio Export Thread" }; // the thread that will write our audio data to disk
    std::unique_ptr<AudioFormatWriter::ThreadedWriter> m_threadedWriter; // the FIFO used to buffer the incoming data
    double m_fSampleRate = 0.0;
    int m_iNumChannels = 1;
    int m_iBitDepth = 16;

    CriticalSection m_writerLock;
    std::atomic<AudioFormatWriter::ThreadedWriter*> m_activeWriter { nullptr };
};
