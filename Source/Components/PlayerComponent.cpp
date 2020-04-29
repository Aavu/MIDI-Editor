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
    initSynth();
    startTimer(Globals::GUI::iUpdateInterval_ms);
}

PlayerComponent::~PlayerComponent()
{
    stopTimer();
}

void PlayerComponent::paint (Graphics& g)
{
}

void PlayerComponent::resized()
{
}

void PlayerComponent::initSynth() {
    File * soundFontFile = new File(CUtil::getAbsolutePathOfProject() + "/Resources/SoundFonts/GeneralUser GS 1.442 MuseScore/GeneralUser GS MuseScore v1.442.sf2");
    m_synth.initSynth(soundFontFile);
    m_synth.addActionListener(this);
}

void PlayerComponent::addMessageToTempoBuffer(const MidiMessage& message) {
    auto msgSampleNumber = message.getTimeStamp() * m_fSampleRate; // Seconds to samples
    //std::cout << message.getTimeStamp() << "\t" << msgSampleNumber << "\t" << message.getDescription() << std::endl;
    m_tempoEventBuffer.addEvent(message, msgSampleNumber);
}

void PlayerComponent::addAllTempoMessagesToBuffer() {
    auto numEvents = m_midiMessageSequence->getNumEvents();
    MidiMessageSequence::MidiEventHolder* const * eventHolder = m_midiMessageSequence->begin();
    MidiMessage msg;
    bool firstTempo = true;
    for (int i=0; i<numEvents; i++) {
        msg = eventHolder[i]->message;
        if (msg.isTempoMetaEvent()) {
            addMessageToTempoBuffer(msg);
            if (firstTempo && msg.getTempoSecondsPerQuarterNote() != 0) {
                m_fCurrentTempo = 60 / msg.getTempoSecondsPerQuarterNote();
                firstTempo = false;
            }
        }
    }
    m_pIterator = std::make_unique<MidiBuffer::Iterator>(m_tempoEventBuffer);
}

void PlayerComponent::fillMidiBuffer(int iNumSamples) {
    MidiMessage msg;
    // Retrieve samples to fill at least one next block
    while(m_iLastRetrievedPosition < (m_iCurrentPosition + iNumSamples)) {
        if (m_iMidiEventReadIdx < m_iMaxMidiEvents) {
            msg = m_midiMessageSequence->getEventPointer(m_iMidiEventReadIdx)->message;
            auto msgSampleNum = static_cast<long> (msg.getTimeStamp() * m_fSampleRate);
            m_midiBuffer.addEvent(msg, msgSampleNum);
            m_iLastRetrievedPosition = msgSampleNum;
            m_iMidiEventReadIdx++;
        }
        else
            break;
    }
}


void PlayerComponent::setMidiMessageSequence(MidiMessageSequence* midiMsgSeq) {
    sendActionMessage(Globals::ActionMessage::Stop);
    m_midiMessageSequence = midiMsgSeq;
    m_midiMessageSequence->sort();

    resetCurrentPosition();
    m_iMaxMidiEvents = m_midiMessageSequence->getNumEvents();

    m_midiBuffer.clear();
    m_iMaxBufferLength = static_cast<long>(m_midiMessageSequence->getEndTime() * m_fSampleRate);

    addAllTempoMessagesToBuffer();
}

void PlayerComponent::play() {
    m_playState = PlayState::Playing;
}

void PlayerComponent::pause() {
    m_playState = PlayState::Paused;
}

void PlayerComponent::stop() {
    m_playState = PlayState::Stopped;
    resetCurrentPosition();
    allNotesOff();
}

void PlayerComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    m_iSamplesPerBlockExpected = samplesPerBlockExpected;
    m_fSampleRate = sampleRate;
    m_synth.setCurrentPlaybackSampleRate(sampleRate);
}

void PlayerComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    m_currentMidiBuffer.clear();
    auto blockSize = bufferToFill.numSamples;

    if (m_playState == PlayState::Playing) {

        // If not enough samples in m_midiBuffer for new block
        if ((m_iCurrentPosition + blockSize) > m_iLastRetrievedPosition) {
            fillMidiBuffer(blockSize);
        }

        if (m_midiBuffer.isEmpty()) {
            m_iCurrentPosition += blockSize;
            return;
        }

        m_currentMidiBuffer.addEvents(m_midiBuffer, m_iCurrentPosition, blockSize, 0);
        m_synth.renderNextBlock (*bufferToFill.buffer, m_currentMidiBuffer, 0, blockSize);

        // Tempo update
        updateTempo();

        m_iCurrentPosition += blockSize;

        if (m_iCurrentPosition > m_iMaxBufferLength) {
            sendActionMessage(Globals::ActionMessage::Stop);
        }
    }
}

void PlayerComponent::updateTempo() {
    if (!m_pIterator)
        return;
    m_pIterator->setNextSamplePosition((int) m_iCurrentPosition);
    MidiMessage result;
    int samplePosition;
    m_pIterator->getNextEvent(result, samplePosition);
    if (result.getTempoSecondsPerQuarterNote() != 0)
        m_fCurrentTempo = 60 / result.getTempoSecondsPerQuarterNote();
}

void PlayerComponent::allNotesOff() {
    for (int i=0; i<SoundFontGeneralMidiSynth::kiNumChannels; i++)
        m_synth.allNotesOff(0, false);
}

void PlayerComponent::setCurrentPosition(long value) {
    m_iCurrentPosition = value;
    updateTempo();
}

void PlayerComponent::resetCurrentPosition() {
    setCurrentPosition(0);
    m_iMidiEventReadIdx = 0;
    m_iLastRetrievedPosition = 0;
}

void PlayerComponent::timerCallback() {
    updateTransportDisplay();
}

void PlayerComponent::actionListenerCallback (const String& message) {
    if (message == Globals::ActionMessage::EnableTransport) // Just relay message to Transport
        sendActionMessage(message);
}