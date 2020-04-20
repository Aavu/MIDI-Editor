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

void PlayerComponent::addMessageToBuffer(const MidiMessage& message) {
    auto msgSampleNumber = message.getTimeStamp() * m_fSampleRate; // Seconds to samples
    //std::cout << message.getTimeStamp() << "\t" << msgSampleNumber << "\t" << message.getDescription() << std::endl;
    m_buffer.addEvent(message, msgSampleNumber);
}

void PlayerComponent::addAllSequenceMessagesToBuffer() {
    auto numEvents = m_midiMessageSequence->getNumEvents();
    MidiMessageSequence::MidiEventHolder* const * eventHolder = m_midiMessageSequence->begin();
    MidiMessage msg;
    for (int i=0; i<numEvents; i++) {
        msg = eventHolder[i]->message;
        if (msg.isNoteOnOrOff()) {
            addMessageToBuffer(msg);
        }
    }

    m_pIterator = std::make_unique<MidiBuffer::Iterator>(m_buffer);
}

void PlayerComponent::setMidiMessageSequence(const MidiMessageSequence* midiMsgSeq) {
    m_midiMessageSequence = midiMsgSeq;
    m_buffer.clear();
    addAllSequenceMessagesToBuffer();
}

void PlayerComponent::play() {
    m_playState = PlayState::Playing;
}

void PlayerComponent::pause() {
    m_playState = PlayState::Paused;
    // TODO: Make sure to flush note ons.
}

void PlayerComponent::stop() {
    m_playState = PlayState::Stopped;
    // TODO: Make sure to flush note ons.
    resetCurrentPosition();
}

void PlayerComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    m_iSamplesPerBlockExpected = samplesPerBlockExpected;
    m_fSampleRate = sampleRate;
}

void PlayerComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();

    if (m_playState == PlayState::Playing)
        updateNumSamples(bufferToFill.numSamples);
}

void PlayerComponent::updateNumSamples(int bufferSize) {
    if (m_buffer.isEmpty()) {
        m_iCurrentPosition += bufferSize;
        return;
    }

    MidiMessage msg;
    int sampleNumber;

    while (m_pIterator->getNextEvent(msg, sampleNumber)) {
//        DBG(sampleNumber << " " << m_iCurrentPosition);
        if (sampleNumber > m_iCurrentPosition)
            break;

        if (msg.isNoteOnOrOff())
            std::cout   << sampleNumber << " --- "
                        << msg.getTimeStamp() << " --- "
                        << msg.getDescription()
                        << std::endl;
    }

    m_pIterator->setNextSamplePosition(m_iCurrentPosition);
    m_iCurrentPosition += bufferSize;
}

void PlayerComponent::resetCurrentPosition() {
    m_iCurrentPosition = 0;
    m_pIterator->setNextSamplePosition(m_iCurrentPosition);
}

PlayerComponent::PlayState PlayerComponent::getPlayState() {
    return m_playState;
}
