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

void PlayerComponent::addMessageToBuffer(const MidiMessage& message) {
    auto msgSampleNumber = message.getTimeStamp() * m_fSampleRate; // Seconds to samples
    //std::cout << message.getTimeStamp() << "\t" << msgSampleNumber << "\t" << message.getDescription() << std::endl;
    m_midiBuffer.addEvent(message, msgSampleNumber);
}

void PlayerComponent::addMessageToTempoBuffer(const MidiMessage& message) {
    auto msgSampleNumber = message.getTimeStamp() * m_fSampleRate; // Seconds to samples
    //std::cout << message.getTimeStamp() << "\t" << msgSampleNumber << "\t" << message.getDescription() << std::endl;
    m_tempoEventBuffer.addEvent(message, msgSampleNumber);
}

void PlayerComponent::addAllSequenceMessagesToBuffer() {
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
        } else {
            addMessageToBuffer(msg);
        }
    }

    m_pIterator = std::make_unique<MidiBuffer::Iterator>(m_tempoEventBuffer);
    m_iMaxBufferLength = m_midiBuffer.getLastEventTime();

//    DBG("max length : " << m_iMaxBufferLength);
}

void PlayerComponent::setMidiMessageSequence(const MidiMessageSequence* midiMsgSeq) {
    sendActionMessage(Globals::ActionMessage::Stop);
    m_midiMessageSequence = midiMsgSeq;
    m_midiBuffer.clear();
    addAllSequenceMessagesToBuffer();
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

    auto numSamples = bufferToFill.numSamples;

    if (m_playState == PlayState::Playing) {
        if (m_midiBuffer.isEmpty()) {
            m_iCurrentPosition += numSamples;
            return;
        }

        m_currentMidiBuffer.addEvents(m_midiBuffer, (int)m_iCurrentPosition, numSamples, 0);
        m_synth.renderNextBlock (*bufferToFill.buffer, m_currentMidiBuffer, 0, numSamples);

        // Tempo update
        updateTempo();

        m_iCurrentPosition += numSamples;

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
        m_synth.allNotesOff(0, true);
}

void PlayerComponent::setCurrentPosition(long value) {
    m_iCurrentPosition = value;
    updateTempo();
}

void PlayerComponent::resetCurrentPosition() {
    setCurrentPosition(0);
}

void PlayerComponent::timerCallback() {
    updateTransportDisplay();
}

void PlayerComponent::actionListenerCallback (const String& message) {
    if (message == Globals::ActionMessage::EnableTransport) // Just relay message to Transport
        sendActionMessage(message);
}