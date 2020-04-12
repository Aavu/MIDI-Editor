/*
  ==============================================================================

    MidiSynth.cpp
    Created: 21 Feb 2020 2:39:12pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#include "MidiSynth.h"


/*
  ==============================================================================

    MIDISynth.cpp
    Created: 21 Feb 2020 12:05:18pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#include "MIDISynth.h"


SineWaveVoice::SineWaveVoice() = default;

bool SineWaveVoice::canPlaySound(SynthesiserSound *sound) {
    return dynamic_cast<SineWaveSound*> (sound) != nullptr;
}

void
SineWaveVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int /*currentPitchWheelPosition*/) {
    currentAngle = 0.0;
    level = velocity * 0.15;
    tailOff = 0.0;
    auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);  // F
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();  // F / Fs
    angleDelta = 2.0 * MathConstants<double>::pi * cyclesPerSample;

}

void SineWaveVoice::stopNote(float /*velocity*/, bool allowTailOff) {
    if (allowTailOff)
    {
        if (tailOff == 0.0)
            tailOff = 1.0;
    }
    else
    {
        clearCurrentNote();
        angleDelta = 0.0;
    }
}

void SineWaveVoice::pitchWheelMoved(int newPitchWheelValue) {}

void SineWaveVoice::controllerMoved(int controllerNumber, int newControllerValue) {}

void SineWaveVoice::renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) {
    if (angleDelta != 0.0)
    {
        if (tailOff > 0.0)
        {
            while (--numSamples >=0)
            {
                auto currentSample = static_cast<float> (std::sin(currentAngle) * level * tailOff); // Generate sine sample
                for (auto c = outputBuffer.getNumChannels(); --c >= 0;)
                    outputBuffer.addSample(c, startSample, currentSample);
                currentAngle += angleDelta;
                ++startSample;
                tailOff *= 0.99; // Tail dies out exponentially;
                if (tailOff <= 0.005)  // Stops after ~527 samples (0.99^527 ~ 0.005)
                {
                    clearCurrentNote();
                    angleDelta = 0.0;
                    break;
                }
            }
        }
        else // no tail
        {
            while (--numSamples >=0)
            {
                auto currentSample = static_cast<float> (std::sin(currentAngle) * level); // Generate sine sample
                for (auto c=outputBuffer.getNumChannels(); --c >= 0;)
                    outputBuffer.addSample(c, startSample, currentSample);
                currentAngle += angleDelta;
                ++startSample;
            }
        }
    }
}


SineWaveSound::SineWaveSound() = default;

bool SineWaveSound::appliesToNote(int midiNoteNumber) {
    return true;
}

bool SineWaveSound::appliesToChannel(int midiChannel) {
    return true;
}

SynthAudioSource::SynthAudioSource(MidiKeyboardState &keyState) :
    m_keyboardState(keyState)
{
    for (auto i=0; i<4; ++i)
        m_synth.addVoice(new SineWaveVoice());  //TODO: Add options to use voices other than sine.
    m_synth.addSound(new SineWaveSound());
}

void SynthAudioSource::setUsingSineWaveSound() {
    m_synth.clearSounds();  //TODO: This seems incomplete. Shouldnt you do m_synth.addSound(new SineWaveSound()) here ??
}

void SynthAudioSource::prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) {
    m_synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SynthAudioSource::releaseResources() {}

void SynthAudioSource::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    m_keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
    m_synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
}

