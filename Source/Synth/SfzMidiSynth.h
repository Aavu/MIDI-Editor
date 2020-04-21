/*
  ==============================================================================

    SfzMidiSynth.h
    Created: 13 Mar 2020 7:08:24pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

class SfzSynth : public sfzero::Synth
{
public:
    SfzSynth();

    void setUsingSineWaveSound();
    void addSound(sfzero::Sound *sound);
    int getProgramNumber() const;
    juce::String getProgramName() const;
    void setProgramNumber(int iProgramNumber);

private:
    sfzero::Sound * getSound() const;
};


class SfzLoader {
public:
    SfzLoader();
    void setSfzFile(File *pNewSfzFile);
    void loadSound(bool bUseLoaderThread = false, std::function<void()> *callback = nullptr);
    double getLoadProgress() const;
    sfzero::Sound * getLoadedSound() const;  // TODO: create factory for this and return new sound object every time.

private:
    void load(Thread *pThread = nullptr);

    class LoadThread : public Thread {
    public:
        explicit LoadThread(SfzLoader *pSfzLoader);
        void run() override;
    private:
        SfzLoader *m_pSfzLoader;
    };
    friend class LoadThread; //TODO: Is this required?? Why?

    File m_sfzFile;
    AudioFormatManager m_formatManager;
    LoadThread m_loadThread;
    sfzero::Sound * m_pSound;
    double m_fLoadProgress;
    std::function<void()> m_callback;
};
