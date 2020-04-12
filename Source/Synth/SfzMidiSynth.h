/*
  ==============================================================================

    SfzMidiSynth.h
    Created: 13 Mar 2020 7:08:24pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

class SfzSynthAudioSource : public AudioSource
{
public:
    explicit SfzSynthAudioSource(MidiKeyboardState& keyState);
    void setUsingSineWaveSound();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;
    //------------------------------SFZero----------------------------------------

    void setSfzFile(File *newSfzFile);
    void setSfzFileThreaded(File *newSfzFile);

    void loadSound(Thread *thread = nullptr);

private:
    MidiKeyboardState& m_keyboardState;
    sfzero::Synth m_synth;

    //------------------------------SFZero----------------------------------------

    class LoadThread : public Thread
    {
    public:
        LoadThread(SfzSynthAudioSource *sfzSynthAudioSrc);
        void run() override;
    protected:
        SfzSynthAudioSource *m_pSfzSynthAudioSource;
    };

    friend class LoadThread;

    double m_fLoadProgress;
    File m_sfzFile;
    AudioFormatManager m_formatManager;
    LoadThread m_loadThread;
};