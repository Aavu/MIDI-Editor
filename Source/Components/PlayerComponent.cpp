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

void PlayerComponent::initSynth() {
    m_synth.clearVoices();

    for (int i=0; i < kiNumVoices; i++) {
        m_synth.addVoice(new sfzero::Voice());
    }

    // Load sound from SoundFont file and add to synth.
    File * soundFontFile = new File(getAbsolutePathOfProject() + "/Resources/SoundFonts/GeneralUser GS 1.442 MuseScore/GeneralUser GS MuseScore v1.442.sf2");

    m_sfzLoader.setSfzFile(soundFontFile);
    std::function<void()> addLoadedSoundCallback = [this] () {
        auto sounds = m_sfzLoader.getLoadedSounds();
        for (auto i=0; i<sounds.size(); i++) {
            auto * sound = sounds.getUnchecked(i).get();
            if (i == 10)
                sound->useSubsound(247);
            sound->setChannelNum(i);
            m_synth.addSound(sound);
        }
        std::cout << sounds.size() << " sounds added." << std::endl;
    };
    m_sfzLoader.loadSounds(kiNumChannels, true, &addLoadedSoundCallback);

}

//void PlayerComponent::addMessageToBuffer(const MidiMessage& message) {
//    auto msgSampleNumber = message.getTimeStamp() * m_fSampleRate; // Seconds to samples
//    //std::cout << message.getTimeStamp() << "\t" << msgSampleNumber << "\t" << message.getDescription() << std::endl;
//    m_midiBuffer.addEvent(message, msgSampleNumber);
//}
//
//void PlayerComponent::addAllSequenceMessagesToBuffer() {
//    auto numEvents = m_midiMessageSequence->getNumEvents();
//    MidiMessageSequence::MidiEventHolder* const * eventHolder = m_midiMessageSequence->begin();
//    MidiMessage msg;
//    for (int i=0; i<numEvents; i++) {
//        msg = eventHolder[i]->message;
//        addMessageToBuffer(msg);
//    }
//    m_pIterator = std::make_unique<MidiBuffer::Iterator>(m_midiBuffer);
//    m_iMaxBufferLength = m_midiBuffer.getLastEventTime();
//    DBG("max length : " << m_iMaxBufferLength);
//}

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
    for (int i=0; i<16; i++)
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

void PlayerComponent::updateNoteTimestamp(int iEventIndex, double fNewTimestamp) {
    auto * pEventAtReadIdx = m_midiMessageSequence->getEventPointer(m_iMidiEventReadIdx);
    DBG("-------------updateNoteTimestamp--------------------");
    DBG(pEventAtReadIdx->message.getDescription());
    m_midiMessageSequence->getEventPointer(iEventIndex)->message.setTimeStamp(fNewTimestamp);
    m_midiMessageSequence->sort();
    DBG(pEventAtReadIdx->message.getDescription());
    m_iMidiEventReadIdx = m_midiMessageSequence->getIndexOf(pEventAtReadIdx);
    DBG("----------------------------------------------------");
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