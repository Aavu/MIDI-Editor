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
//: m_synthAudioSource(m_keyBoardState)
{
//    m_synthAudioSource.setSfzFile(new File(getAbsolutePathOfProject() + "/Resources/SoundFonts/GeneralUser GS 1.442 MuseScore/GeneralUser GS MuseScore v1.442.sf2"));
//    m_keyBoardState.reset();
    initSynth();
}

PlayerComponent::~PlayerComponent()
{
//    m_synthAudioSource.releaseResources();
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
        m_synth.addVoice(new SynthVoice());
    }

    m_synth.clearSounds();
    m_synth.addSound(new SynthSound());
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
//    m_synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    m_synth.setCurrentPlaybackSampleRate(sampleRate);
}

void PlayerComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    if (m_playState == PlayState::Playing) {
        updateNumSamples(bufferToFill);
    }

    bufferToFill.clearActiveBufferRegion();
}

void PlayerComponent::updateNumSamples(const AudioSourceChannelInfo &bufferToFill) {
    if (m_midiBuffer.isEmpty()) {
        m_iCurrentPosition += bufferToFill.numSamples;
        return;
    }

    MidiMessage msg;
    int sampleNumber;
    m_currentMidiBuffer.clear();
    m_currentMidiBuffer.addEvents(m_midiBuffer, m_iCurrentPosition, bufferToFill.numSamples, 0);

    m_synth.renderNextBlock (*bufferToFill.buffer, m_currentMidiBuffer, 0, bufferToFill.numSamples);

//    const ScopedLock sl (lock);
//
//    while (m_pIterator->getNextEvent(msg, sampleNumber)) {
////        DBG(sampleNumber << " " << m_iCurrentPosition);
//        if (sampleNumber > m_iCurrentPosition)
//            break;
//
//
//        if (msg.isNoteOn()) {
//            m_synth.noteOn(msg.getChannel(), msg.getNoteNumber(), msg.getFloatVelocity());
////            m_synthAudioSource.handleNoteOn(msg);
//            std::cout << sampleNumber << " --- "
//                      << msg.getTimeStamp() << " --- "
//                      << msg.getDescription()
//                      << std::endl;
//        }
//
//        if (msg.isNoteOff()) {
//            m_synth.noteOff(msg.getChannel(), msg.getNoteNumber(), msg.getFloatVelocity(), true);
////            m_synthAudioSource.handleNoteOff(msg);
//        }
//    }
//
//    m_pIterator->setNextSamplePosition(m_iCurrentPosition);
    m_iCurrentPosition += bufferToFill.numSamples;
}

void PlayerComponent::resetCurrentPosition() {
    m_iCurrentPosition = 0;
    m_pIterator->setNextSamplePosition(m_iCurrentPosition);
}

PlayerComponent::PlayState PlayerComponent::getPlayState() {
    return m_playState;
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