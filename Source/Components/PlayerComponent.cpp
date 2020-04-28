/*
  ==============================================================================

    PlayerComponent.cpp
    Created: 12 Jan 2020 8:47:56pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlayerComponent.h"


static double convertTicksToSeconds (double time,
                                     const MidiMessageSequence& tempoEvents,
                                     int timeFormat)
{
    if (timeFormat < 0)
        return time / (-(timeFormat >> 8) * (timeFormat & 0xff));
    
    double lastTime = 0, correctedTime = 0;
    auto tickLen = 1.0 / (timeFormat & 0x7fff);
    auto secsPerTick = 0.5 * tickLen;
    auto numEvents = tempoEvents.getNumEvents();
    
    for (int i = 0; i < numEvents; ++i)
    {
        auto& m = tempoEvents.getEventPointer(i)->message;
        auto eventTime = m.getTimeStamp();
        
        if (eventTime >= time)
            break;
        
        correctedTime += (eventTime - lastTime) * secsPerTick;
        lastTime = eventTime;
        
        if (m.isTempoMetaEvent())
            secsPerTick = tickLen * m.getTempoSecondsPerQuarterNote();
        
        while (i + 1 < numEvents)
        {
            auto& m2 = tempoEvents.getEventPointer(i + 1)->message;
            
            if (m2.getTimeStamp() != eventTime)
                break;
            
            if (m2.isTempoMetaEvent())
                secsPerTick = tickLen * m2.getTempoSecondsPerQuarterNote();
            
            ++i;
        }
    }
    
    return correctedTime + (time - lastTime) * secsPerTick;
}

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
            DBG("--------------------------------");
            msg = m_midiMessageSequence->getEventPointer(m_iMidiEventReadIdx)->message;
            DBG("Msg timestamp" << msg.getTimeStamp());
            auto msgSampleNum = static_cast<long> (msg.getTimeStamp() * m_fSampleRate);
            DBG("Msg sample num: " << msgSampleNum);
            DBG("ReadIdx: " << m_iMidiEventReadIdx);
            m_midiBuffer.addEvent(msg, msgSampleNum);
            m_iLastRetrievedPosition = msgSampleNum;
            m_iMidiEventReadIdx++;
            DBG("--------------------------------");
        }
        else
            break;

    }
    DBG("iRetrievedSamples = " << m_iLastRetrievedPosition - m_iCurrentPosition);
    DBG("----------------------------------------------------");
}

MidiMessageSequence& PlayerComponent::getTempoEvents()
{
    return m_TempoEvents;
}

MidiMessageSequence& PlayerComponent::getTempoEventsInSecs()
{
    return m_TempoEventsInSec;
}

double PlayerComponent::convertQuarterNoteToSec(double positionInQuarterNotes)
{
    double positionInSec = 0;
    if (m_TempoEvents.getNumEvents() > 0)
    {
        double target_tick = positionInQuarterNotes * m_iTimeFormat;
        double cur_tempo = m_TempoEvents.getEventPointer(0)->message.getTempoSecondsPerQuarterNote();
        int st_tick = 0;
        double st_sec = 0;
        
        // m_TempoEvents is sorted by time
        // m_tempoEventsInSec has the same order as m_TempoEvents
        for (int i = 1; i < m_TempoEvents.getNumEvents(); i++)
        {
            MidiMessage c_message =m_TempoEventsInSec.getEventPointer(i)->message;
            double c_timeInTick = m_TempoEvents.getEventTime(i);
            
            // DBG(String(c_timeInSec) + " " + String(c_message.getTimeStamp()) + " " + String(60/c_tempo));
            
            if (target_tick >= c_timeInTick)
            {
                cur_tempo = c_message.getTempoSecondsPerQuarterNote();
                st_tick = c_timeInTick;
                st_sec = c_message.getTimeStamp();
            }
            else
            {
                break;
            }
        }
        positionInSec = st_sec + (target_tick-st_tick) * cur_tempo / m_iTimeFormat;
    }
    return positionInSec;
}

double PlayerComponent::getCurrentPositionInQuarterNotes()
{
    double curPositionInQuarterNotes = 0;
    if (m_TempoEvents.getNumEvents() > 0)
    {
        double target_sec = m_iCurrentPosition/m_fSampleRate;
        double cur_tempo = m_TempoEvents.getEventPointer(0)->message.getTempoSecondsPerQuarterNote();
        int st_tick = 0;
        double st_sec = 0;
        
        // m_TempoEvents is sorted by time
        // m_tempoEventsInSec has the same order as m_TempoEvents
        for (int i = 1; i < m_TempoEvents.getNumEvents(); i++)
        {
            MidiMessage c_message =m_TempoEvents.getEventPointer(i)->message;
            double c_timeInSec = m_TempoEventsInSec.getEventTime(i);
            
            // DBG(String(c_timeInSec) + " " + String(c_message.getTimeStamp()) + " " + String(60/c_tempo));
            
            if (target_sec >= c_timeInSec)
            {
                cur_tempo = c_message.getTempoSecondsPerQuarterNote();
                st_tick = c_message.getTimeStamp();
                st_sec = c_timeInSec;
            }
            else
            {
                break;
            }
        }
        curPositionInQuarterNotes = st_tick / m_iTimeFormat + (target_sec-st_sec) / cur_tempo;
    }
    return curPositionInQuarterNotes;
}

void PlayerComponent::setCurrentPositionByQuarterNotes(double newPositionInQuarterNotes)
{
    double newPositionInSamples = convertQuarterNoteToSec(newPositionInQuarterNotes) * m_fSampleRate;
    m_iCurrentPosition = static_cast<long>(newPositionInSamples);
    return;
}

void PlayerComponent::setTimeFormat(int timeFormat)
{
    m_iTimeFormat = timeFormat;
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
    //resetCurrentPosition();
    setCurrentPositionByQuarterNotes(0);
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
        } else {
         //   DBG("-----" << (m_iCurrentPosition + blockSize) << "-- " << m_iLastRetrievedPosition);
        }

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

void PlayerComponent::updateNoteTimestamps(int iNoteOnEventIndex, double fNewNoteOnTimestampInQuarterNote, double fNoteDurationInQuarterNote /*= -1*/) {
    DBG("-------------updateNoteTimestamp--------------------");

    auto * pEventAtReadIdx = m_midiMessageSequence->getEventPointer(m_iMidiEventReadIdx); // To maintain read index after sort
    DBG(pEventAtReadIdx->message.getDescription() + String(pEventAtReadIdx->message.getTimeStamp()));

    // Get noteOn and noteOff events
    auto * pNoteOnEvent = m_midiMessageSequence->getEventPointer(iNoteOnEventIndex);
    auto * pNoteOffEvent = m_midiMessageSequence->getEventPointer(m_midiMessageSequence->getIndexOfMatchingKeyUp(iNoteOnEventIndex));

    // Update timestamps for both noteOn and noteOff
    auto fNewNoteOnTimestamp = convertQuarterNoteToSec(fNewNoteOnTimestampInQuarterNote);
    double fNoteDuration;
    if (fNoteDurationInQuarterNote == -1) // keep duration same as before
        fNoteDuration = pNoteOffEvent->message.getTimeStamp() - pNoteOnEvent->message.getTimeStamp();
    else
        fNoteDuration = convertQuarterNoteToSec(fNewNoteOnTimestamp);
    pNoteOnEvent->message.setTimeStamp(fNewNoteOnTimestamp);
    pNoteOffEvent->message.setTimeStamp(fNewNoteOnTimestamp + fNoteDuration);

    // Re-order MidiMessageSeuence
    m_midiMessageSequence->updateMatchedPairs(); //TODO: Are both updateMatchedPairs and sort required ???
    m_midiMessageSequence->sort();

    // Set read index back to correct position after re-ordering.
    DBG(pEventAtReadIdx->message.getDescription() + String(pEventAtReadIdx->message.getTimeStamp()));
    m_iMidiEventReadIdx = m_midiMessageSequence->getIndexOf(pEventAtReadIdx); // why do you change that?
    
    DBG("----------------------------------------------------");
}

void PlayerComponent::updateNotePitch(int iNoteOnEventIndex, int iNewNoteNumber) {

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
