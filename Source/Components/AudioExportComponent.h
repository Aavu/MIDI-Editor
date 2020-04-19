//
// Created by Raghavasimhan Sankaranarayanan on 4/18/20.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class AudioExportComponent: public Component, public ActionListener {
public:
    explicit AudioExportComponent(double sampleRate, int channels, int bitDepth) :
    m_fSampleRate(sampleRate),
    m_iNumChannels(channels),
    m_iBitDepth(bitDepth)
    {
        m_backgroundThread.startThread();
    }

    ~AudioExportComponent() override {
        finish();
    }

    void init(const File& file) {
        if (auto fileStream = std::unique_ptr<FileOutputStream> (file.createOutputStream())) {
            WavAudioFormat wavFormat;
            if (auto writer = wavFormat.createWriterFor(fileStream.get(), m_fSampleRate, m_iNumChannels, m_iBitDepth, {}, 0)) {
                auto _ = fileStream.release();
                m_threadedWriter = std::make_unique<AudioFormatWriter::ThreadedWriter>(writer, m_backgroundThread, 32768);
//                m_threadedWriter.reset(new AudioFormatWriter::ThreadedWriter(writer, m_backgroundThread, m_iSamplesPerBlockExpected));
                m_iNextSampleNum = 0;

                const ScopedLock sl (m_writerLock);
                m_activeWriter = m_threadedWriter.get();
            }
        }
    }

    void write(const AudioSourceChannelInfo& bufferToFill) {
        const ScopedLock sl (m_writerLock);
        if (m_activeWriter.load() != nullptr)
        {
            m_activeWriter.load()->write (bufferToFill.buffer->getArrayOfReadPointers(), bufferToFill.numSamples);
            m_iNextSampleNum += bufferToFill.numSamples;
        }
    }

    void finish() {
        {
            const ScopedLock sl (m_writerLock);
            m_activeWriter = nullptr;
        }

        m_threadedWriter.reset();
    }

    bool isExporting() const
    {
        return m_activeWriter.load() != nullptr;
    }

private:
    void actionListenerCallback (const String& message) override {
        if (message == "finishExport") {
            std::cout << message << std::endl;
            finish();
        }
    }

    TimeSliceThread m_backgroundThread { "Audio Export Thread" }; // the thread that will write our audio data to disk
    std::unique_ptr<AudioFormatWriter::ThreadedWriter> m_threadedWriter; // the FIFO used to buffer the incoming data
    double m_fSampleRate = 0.0;
    int m_iNumChannels = 1;
    int m_iBitDepth = 16;
    int64 m_iNextSampleNum = 0;

    CriticalSection m_writerLock;
    std::atomic<AudioFormatWriter::ThreadedWriter*> m_activeWriter { nullptr };
};
