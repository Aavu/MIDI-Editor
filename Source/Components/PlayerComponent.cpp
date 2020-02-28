/*
  ==============================================================================

    PlayerComponent.cpp
    Created: 12 Jan 2020 8:47:56pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlayerComponent.h"

//==============================================================================
PlayerComponent::PlayerComponent()
{
}

PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::paint (Graphics& g)
{
}

void PlayerComponent::resized()
{
}

void PlayerComponent::resetStartTime() {
    startTime = Time::getMillisecondCounterHiRes() * 0.001;
}

void PlayerComponent::startPlayerTimer() {
    if (midiMessageSequence)
        startTimer(1);
    else
        ; //TODO: Throw uninit exception
}

void PlayerComponent::stopPlayerTimer() {
    stopTimer();
}

void PlayerComponent::addMessageToBuffer(const MidiMessage& message) {
    auto msgSampleNumber = message.getTimeStamp() * sampleRate; // Seconds to samples
    //std::cout << message.getTimeStamp() << "\t" << msgSampleNumber << "\t" << message.getDescription() << std::endl;
    buffer.addEvent(message, msgSampleNumber);
}

void PlayerComponent::addAllSequenceMessagesToBuffer() {
    auto numEvents = midiMessageSequence->getNumEvents();
    MidiMessageSequence::MidiEventHolder* const * eventHolder = midiMessageSequence->begin();
    MidiMessage msg;
    for (int i=0; i<numEvents; i++) {
        msg = eventHolder[i]->message;
        if (msg.isNoteOnOrOff()) {
            addMessageToBuffer(msg);
        }
    }
}

void PlayerComponent::setMidiMessageSequence(const MidiMessageSequence* midiMsgSeq) {
    midiMessageSequence = midiMsgSeq;
    addAllSequenceMessagesToBuffer();
}

void PlayerComponent::timerCallback() {
    auto currentTime = Time::getMillisecondCounterHiRes() * 0.001 - startTime;
    auto currentSampleNumber = currentTime * sampleRate;

    MidiBuffer::Iterator iterator(buffer);
    MidiMessage msg;
    int sampleNumber;

    while (iterator.getNextEvent(msg, sampleNumber))
    {
        if (sampleNumber > currentSampleNumber)
            break;
        if (msg.isNoteOnOrOff())
            std::cout <<  sampleNumber << "---" <<  msg.getTimeStamp() << "---" << msg.getDescription() << std::endl;
    }
    buffer.clear(previousSampleNumber, (int)currentSampleNumber - previousSampleNumber);

    if (buffer.getNumEvents() <= 0) {
        std::cout << "Finished Playing." <<  std::endl;
        stopPlayerTimer();
    }

    previousSampleNumber = currentSampleNumber;
}

void PlayerComponent::resetPlayHead() {
    stopPlayerTimer();
    previousSampleNumber = 0;
    buffer.clear();
    addAllSequenceMessagesToBuffer();
}

void PlayerComponent::play() {
    resetPlayHead();
    resetStartTime();
    startPlayerTimer();
}

