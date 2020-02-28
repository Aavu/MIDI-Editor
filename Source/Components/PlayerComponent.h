/*
  ==============================================================================

    PlayerComponent.h
    Created: 12 Jan 2020 8:47:56pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/

//TODO: Implement play pause stop states.
//TODO: Use separate thread for timing instead of Timer class.?

class PlayerComponent    : public Component, private Timer
{
public:
    PlayerComponent();
    ~PlayerComponent() override;

    void paint (Graphics&) override;
    void resized() override;


    void setMidiMessageSequence(const MidiMessageSequence* midiMsgSeq);
    void resetPlayHead();
    void play(); //This currently restarts playback.
    //TODO: pause, stop

private:
    void timerCallback() override;
    void addMessageToBuffer(const MidiMessage& message);
    void addAllSequenceMessagesToBuffer();
    void resetStartTime();
    void startPlayerTimer();
    void stopPlayerTimer();



    const MidiMessageSequence* midiMessageSequence = nullptr;
    MidiBuffer buffer;
    double startTime = 0;
    double sampleRate = 44100; //TODO: Set through constructor / setter?
    double previousSampleNumber = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerComponent)
};
