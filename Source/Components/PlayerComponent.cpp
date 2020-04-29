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
    File * soundFontFile = new File(getAbsolutePathOfProject() + "/Resources/SoundFonts/GeneralUser GS 1.442 MuseScore/GeneralUser GS MuseScore v1.442.sf2");
    m_synth.initSynth(soundFontFile);
    m_synth.addActionListener(this);
}

void PlayerComponent::fillMidiBuffer(int iNumSamples) {
    DBG("----PlayerComponent::fillMidiBuffer--------------------------------");
    MidiMessage msg;
    DBG("Max midi events: " << m_iMaxMidiEvents);
    // Retrieve samples to fill at least one next block
    while(m_iLastRetrievedPosition < (m_iCurrentPosition + iNumSamples)) {
        if (m_iMidiEventReadIdx < m_iMaxMidiEvents) {
            msg = m_midiMessageSequence->getEventPointer(m_iMidiEventReadIdx)->message;
            DBG(msg.getTimeStamp());
            auto msgSampleNum = static_cast<long> (msg.getTimeStamp() * m_fSampleRate);
            DBG(msgSampleNum << "==" << m_iMidiEventReadIdx);
            m_midiBuffer.addEvent(msg, msgSampleNum);
            m_iLastRetrievedPosition = msgSampleNum;
            m_iMidiEventReadIdx++;
        }
        else
            break;
    }

    DBG("iRetrievedSamples = " << m_iLastRetrievedPosition - m_iCurrentPosition);
}


void PlayerComponent::setMidiMessageSequence(MidiMessageSequence* midiMsgSeq) {
    m_midiMessageSequence = midiMsgSeq;
    m_midiMessageSequence->sort();

    m_iMidiEventReadIdx = 0;
    m_iMaxMidiEvents = m_midiMessageSequence->getNumEvents();

    m_midiBuffer.clear();
    m_iMaxBufferLength = static_cast<long>(m_midiMessageSequence->getEndTime() * m_fSampleRate);

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
//        if (m_midiBuffer.isEmpty()) {
//            m_iCurrentPosition += blockSize;
//            return;
//        }


        // If not enough samples in m_midiBuffer for new block
        if ((m_iCurrentPosition + blockSize) > m_iLastRetrievedPosition) {
            DBG("----Need-more-samples-------------------------");
            DBG(m_iCurrentPosition << "---" << m_iLastRetrievedPosition << "---" << m_iLastRetrievedPosition - (m_iCurrentPosition+blockSize));
            fillMidiBuffer(blockSize);
        } else
            DBG("-----" << (m_iCurrentPosition + blockSize) << "-- "<< m_iLastRetrievedPosition);

        m_currentMidiBuffer.addEvents(m_midiBuffer, m_iCurrentPosition, blockSize, 0);
        m_synth.renderNextBlock (*bufferToFill.buffer, m_currentMidiBuffer, 0, blockSize);

        m_iCurrentPosition += blockSize;

        if (m_iCurrentPosition > m_iMaxBufferLength) {
            sendActionMessage(Globals::ActionMessage::Stop);
        }
    }
}

void PlayerComponent::allNotesOff() {
    for (int i=0; i<SoundFontGeneralMidiSynth::kiNumChannels; i++)
        m_synth.allNotesOff(0, true);
}

void PlayerComponent::setCurrentPosition(long value) {
    m_iCurrentPosition = value;
    if (m_pIterator)
        m_pIterator->setNextSamplePosition(m_iCurrentPosition);
}

void PlayerComponent::resetCurrentPosition() {
    setCurrentPosition(0);
}

String PlayerComponent::getAbsolutePathOfProject(const String &projectFolderName) {
    File currentDir = File::getCurrentWorkingDirectory();

    while (currentDir.getFileName() != projectFolderName) {
        currentDir = currentDir.getParentDirectory();
        if (currentDir.getFullPathName() == "/")
            return String();
    }
    return currentDir.getFullPathName();
}

void PlayerComponent::timerCallback() {
    updateTimeDisplay();
}

void PlayerComponent::actionListenerCallback (const String& message) {
    if (message == Globals::ActionMessage::EnableTransport) // Just relay message to Transport
        sendActionMessage(message);
}