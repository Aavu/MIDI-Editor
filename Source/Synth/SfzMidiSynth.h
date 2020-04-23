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

    void handleProgramChange (int iMidiChannel, int iProgram) override ;

    void addSound(sfzero::Sound *pSound);
    int getProgramNumber(int iMidiChannel) const;
    juce::String getProgramName(int iProgram) const;

private:
    sfzero::Sound * getSound(int iMidiChannel) const;
};


class SfzLoader {
public:
    SfzLoader();

    void setSfzFile(File *pNewSfzFile);
    void loadSounds(int iNumInstances = 1, bool bUseLoaderThread = false, std::function<void()> *callback = nullptr);
    double getLoadProgress() const;
    ReferenceCountedArray<sfzero::Sound> getLoadedSounds() const;

private:
    void load(Thread *pThread = nullptr);

    class LoadThread : public Thread {
    public:
        explicit LoadThread(SfzLoader *pSfzLoader);
        void run() override;
    private:
        SfzLoader *m_pSfzLoader;
    };

    File m_sfzFile;
    AudioFormatManager m_formatManager;
    LoadThread m_loadThread;
    ReferenceCountedArray<sfzero::Sound> m_sounds;
    double m_fLoadProgress;
    int m_iNumInstances;
    std::function<void()> m_callback;
};
