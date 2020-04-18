//
// Created by Raghavasimhan Sankaranarayanan on 4/17/20.
//

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"

class SynthVoice : public SynthesiserVoice {
public:
    bool canPlaySound (SynthesiserSound* sound) override {
        return (dynamic_cast<SynthSound*>(sound) != nullptr);
    }

    void startNote (int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override {
        DBG(midiNoteNumber);
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * MathConstants<double>::twoPi;
    }

    void stopNote (float velocity, bool allowTailOff) override {
        if (allowTailOff) {
            if (tailOff == 0.0)
                tailOff = 1.0;
        } else {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved (int newPitchWheelValue) override {}

    void controllerMoved (int controllerNumber, int newControllerValue) override {}

    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override {
        if (angleDelta != 0.0) {
            if (tailOff > 0.0) {
                while (--numSamples >= 0) {
                    auto currentSample = (float) (std::sin (currentAngle) * level * tailOff);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    tailOff *= 0.99;

                    if (tailOff <= 0.005) {
                        clearCurrentNote();

                        angleDelta = 0.0;
                        break;
                    }
                }
            } else {
                while (--numSamples >= 0) {
                    auto currentSample = (float) (std::sin (currentAngle) * level);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }

    using SynthesiserVoice::renderNextBlock;

private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};
