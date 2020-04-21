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

//    m_sfzLoader.setSfzFile(soundFontFile);
//    std::function<void()> addLoadedSoundCallback = [this] () {
//        std::cout << "Sound 0 Loaded: ch 0" << std::endl;
//        auto * sound = m_sfzLoader.getLoadedSound();
//        sound->useSubsound(0);
//        sound->setChannelNum(3);
//        m_synth.addSound(sound);
//    };
 //   m_sfzLoader.loadSound(true, &addLoadedSoundCallback);
    //TODO: add ability to check through all available sounds in synth
    m_sfzLoader1.setSfzFile(soundFontFile);
    std::function<void()> addLoadedSoundCallback1 = [this] () {
        std::cout << "Sound 40 Loaded: ch 3" << std::endl;
        auto * sound = m_sfzLoader1.getLoadedSound();
        sound->useSubsound(40);
        sound->setChannelNum(3);
        m_synth.addSound(sound);
    };
    m_sfzLoader1.loadSound(true, &addLoadedSoundCallback1);

}

void PlayerComponent::addMessageToBuffer(const MidiMessage& message) {
    auto msgSampleNumber = message.getTimeStamp() * m_fSampleRate; // Seconds to samples
    //std::cout << message.getTimeStamp() << "\t" << msgSampleNumber << "\t" << message.getDescription() << std::endl;
    m_midiBuffer.addEvent(message, msgSampleNumber);
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

    m_pIterator = std::make_unique<MidiBuffer::Iterator>(m_midiBuffer);
    m_ulMaxBufferLength = m_midiBuffer.getLastEventTime();
    DBG("max length : " << m_ulMaxBufferLength);
}

void PlayerComponent::setMidiMessageSequence(const MidiMessageSequence* midiMsgSeq) {
    m_midiMessageSequence = midiMsgSeq;
    m_midiBuffer.clear();
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

        m_currentMidiBuffer.addEvents(m_midiBuffer, m_iCurrentPosition, numSamples, 0);
        m_synth.renderNextBlock (*bufferToFill.buffer, m_currentMidiBuffer, 0, numSamples);

        m_iCurrentPosition += numSamples;

        if (m_iCurrentPosition > m_ulMaxBufferLength) {
            sendActionMessage(Globals::ActionMessage::Stop);
        }
    }
}

void PlayerComponent::setCurrentPosition(int value) {
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