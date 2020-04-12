/*
  ==============================================================================

    PlayerComponent.h
    Created: 12 Jan 2020 8:47:56pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <memory>
#include <vector>

//==============================================================================
/*
*/

//TODO: Implement play pause stop states.
//TODO: Use separate thread for timing instead of Timer class.?

class PlayerComponent    : public Component
{
public:
    PlayerComponent();
    ~PlayerComponent() override;

    void paint (Graphics&) override;
    void resized() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);

    void setMidiMessageSequence(const MidiMessageSequence* midiMsgSeq);
    void play();
    void pause();
    void stop();

    enum class PlayState {
        Playing,
        Paused,
        Stopped
    };

    PlayState getPlayState();
    void updateNumSamples(int bufferSize);
    void resetCurrentPosition();

private:
    void addMessageToBuffer(const MidiMessage& message);
    void addAllSequenceMessagesToBuffer();


    const MidiMessageSequence* m_midiMessageSequence = nullptr;
    MidiBuffer m_buffer;
    std::unique_ptr<MidiBuffer::Iterator> m_pIterator;
    double m_fSampleRate = 0;
    int m_iSamplesPerBlockExpected = 0;
    PlayState m_playState = PlayState::Stopped;
    int m_iCurrentPosition = 0;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerComponent)
};
