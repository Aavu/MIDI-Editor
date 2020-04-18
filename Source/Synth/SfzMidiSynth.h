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
    void renderNextBlock(const MidiBuffer& incomingMidi, const AudioSourceChannelInfo &bufferToFill, int startSample);

    void handleNoteOn(const MidiMessage& m);
    void handleNoteOff(const MidiMessage& m);
    //------------------------------SFZero----------------------------------------

    void setSfzFile(File *newSfzFile);
    void setSfzFileThreaded(File *newSfzFile);

    void loadSound(Thread *thread = nullptr);

private:
    MidiKeyboardState& keyboardState;
    sfzero::Synth synth;
    //------------------------------SFZero----------------------------------------

    class LoadThread : public Thread
    {
    public:
        LoadThread(SfzSynthAudioSource *sfzSynthAudioSrc);
        void run() override;
    protected:
        SfzSynthAudioSource *sfzSynthAudioSource;
    };

    friend class LoadThread;

    double loadProgress;
    File sfzFile;
    AudioFormatManager formatManager;
    LoadThread loadThread;
};