/*
  ==============================================================================

    MidiSynth.h
    Created: 21 Feb 2020 2:39:12pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"


class SineWaveSound : public SynthesiserSound
{
public:
    SineWaveSound();
    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToChannel(int midiChannel) override;
};


class SineWaveVoice : public SynthesiserVoice
{
public:
    SineWaveVoice();
    bool canPlaySound(SynthesiserSound *sound) override;
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

private:
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double level = 0.0;
    double tailOff = 0.0;
};


class SynthAudioSource : public AudioSource
{
public:
    explicit SynthAudioSource(MidiKeyboardState& keyState);
    void setUsingSineWaveSound();
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

private:
    MidiKeyboardState& m_keyboardState;
    Synthesiser m_synth; // TODO: Should this be a pointer? Can be initialized later.
};
