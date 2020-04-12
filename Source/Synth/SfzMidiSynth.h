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

    int getProgramNumber() const;
    juce::String getProgramName() const;
    void setProgramNumber(int iProgramNumber);

    void setSfzFile(File *newSfzFile);
    void setSfzFileThreaded(File *newSfzFile);
    void loadSound(Thread *thread = nullptr);

private:
    sfzero::Sound * getSound() const;
    //-------------------------------------

    // TODO: It might be better to load sounds separately using a different class.
    //  Don't want to reload sounds for every synth instance
    friend class LoadThread;
    class LoadThread : public Thread
    {
    public:
        explicit LoadThread(SfzSynthAudioSource *sfzSynthAudioSrc);
        void run() override;
    protected:
        SfzSynthAudioSource *m_pSfzSynthAudioSource;
    };
    //-------------------------------------

    MidiKeyboardState& m_keyboardState;
    sfzero::Synth m_synth; //TODO: use ptr and init()
    File m_sfzFile;
    AudioFormatManager m_formatManager;
    LoadThread m_loadThread;
    double m_fLoadProgress;
};