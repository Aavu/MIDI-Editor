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
//            DBG("--------------------------------");
            msg = m_midiMessageSequence->getEventPointer(m_iMidiEventReadIdx)->message;

            auto msgSampleNum = static_cast<long> (msg.getTimeStamp() * m_fSampleRate);
            m_midiBuffer.addEvent(msg, (int)msgSampleNum);

            m_iLastRetrievedPosition = msgSampleNum;
            m_iMidiEventReadIdx++;
//            DBG("--------------------------------");
        }
        else
            break;
    }
//    DBG("iRetrievedSamples = " << m_iLastRetrievedPosition - m_iCurrentPosition);
//    DBG("----------------------------------------------------");
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
    
    double target_tick = positionInQuarterNotes * m_iTimeFormat;
    double cur_tempo = 60.F / 120;
    int st_tick = 0;
    double st_sec = 0;
    
    if (m_TempoEvents.getNumEvents() > 0)
    {
        cur_tempo = m_TempoEvents.getEventPointer(0)->message.getTempoSecondsPerQuarterNote();
        
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
    }
    positionInSec = st_sec + (target_tick-st_tick) * cur_tempo / m_iTimeFormat;
    //DBG("quarter to sec: " + String(positionInSec));
    return positionInSec;
}

double PlayerComponent::convertSecToQuarterNote(double positionInSec)
{
    double positionInQuarterNotes = 0;
    
    double target_sec = positionInSec;
    double cur_tempo = 60.F/120;
    int st_tick = 0;
    double st_sec = 0;
    
    if (m_TempoEvents.getNumEvents() > 0)
    {
        cur_tempo = m_TempoEvents.getEventPointer(0)->message.getTempoSecondsPerQuarterNote();
        
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
    }
    positionInQuarterNotes = st_tick*1.F / m_iTimeFormat + (target_sec-st_sec) / cur_tempo;
    //DBG("sec to quarter: " + String(positionInQuarterNotes));
    return positionInQuarterNotes;
}

double PlayerComponent::getCurrentPositionInQuarterNotes()
{
    double curPositionInQuarterNotes = convertSecToQuarterNote(m_iCurrentPosition*1.F/m_fSampleRate);
    return curPositionInQuarterNotes;
}

void PlayerComponent::setCurrentPositionByQuarterNotes(double newPositionInQuarterNotes)
{
    double newPositionInSamples = convertQuarterNoteToSec(newPositionInQuarterNotes) * m_fSampleRate;
    m_iCurrentPosition = static_cast<long> (newPositionInSamples);
    return;
}

void PlayerComponent::setTimeFormat(int timeFormat)
{
    m_iTimeFormat = timeFormat;
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
//    DBG("Stop");
    m_midiBuffer.clear();
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
//        else {
//            DBG("-----" << (m_iCurrentPosition + blockSize) << "-- " << m_iLastRetrievedPosition);
//        }

        if (m_midiBuffer.isEmpty()) {
            m_iCurrentPosition += blockSize;
            return;

        }

        m_currentMidiBuffer.addEvents(m_midiBuffer, (int)m_iCurrentPosition, blockSize, 0);
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
    setCurrentPositionByQuarterNotes(0);
    m_iMidiEventReadIdx = 0;
    m_iLastRetrievedPosition = 0;
}

void PlayerComponent::updateNoteTimestamps(int iNoteOnEventIndex, double fNewNoteOnTimestampInQuarterNotes, double fNoteDurationInQuarterNotes /*= -1*/) {
//    DBG("-------------updateNoteTimestamps--------------------");

    auto * pEventAtReadIdx = m_midiMessageSequence->getEventPointer(m_iMidiEventReadIdx); // To maintain read index after sort
//    DBG("EventAtReadIndex: " << pEventAtReadIdx->message.getDescription() << " " <<pEventAtReadIdx->message.getTimeStamp());

    // Get noteOn and noteOff events
    auto * pNoteOnEvent = m_midiMessageSequence->getEventPointer(iNoteOnEventIndex);
    auto * pNoteOffEvent = m_midiMessageSequence->getEventPointer(m_midiMessageSequence->getIndexOfMatchingKeyUp(iNoteOnEventIndex));

    // Update timestamps for both noteOn and noteOff
    auto fNewNoteOnTimestamp = convertQuarterNoteToSec(fNewNoteOnTimestampInQuarterNotes);
    double fNoteDuration;
    if (fNoteDurationInQuarterNotes == -1) // keep duration same as before
        fNoteDuration = pNoteOffEvent->message.getTimeStamp() - pNoteOnEvent->message.getTimeStamp();
    else
        fNoteDuration = convertQuarterNoteToSec(fNewNoteOnTimestamp);
    pNoteOnEvent->message.setTimeStamp(fNewNoteOnTimestamp);
    pNoteOffEvent->message.setTimeStamp(fNewNoteOnTimestamp + fNoteDuration);

    // Re-order MidiMessageSeuence
    m_midiMessageSequence->updateMatchedPairs(); //TODO: Are both updateMatchedPairs and sort required ???
    m_midiMessageSequence->sort();

    // Set read index back to correct position after re-ordering.
//    DBG(pEventAtReadIdx->message.getDescription() + String(pEventAtReadIdx->message.getTimeStamp()));
    m_iMidiEventReadIdx = m_midiMessageSequence->getIndexOf(pEventAtReadIdx); // why do you change that?

//    DBG("----------------------------------------------------");
}

void PlayerComponent::updateNotePitch(int iNoteOnEventIndex, int iNewNoteNumber) {
//    DBG("-------------updateNotePitch--------------------");
    auto * pNoteOnEvent = m_midiMessageSequence->getEventPointer(iNoteOnEventIndex);
    auto * pNoteOffEvent = m_midiMessageSequence->getEventPointer(m_midiMessageSequence->getIndexOfMatchingKeyUp(iNoteOnEventIndex));
    pNoteOnEvent->message.setNoteNumber(iNewNoteNumber);
    pNoteOffEvent->message.setNoteNumber(iNewNoteNumber);
    //TODO: is updateMatchedPairs required here?
//    DBG("----------------------------------------------------");

}

void PlayerComponent::timerCallback() {
    updateTransportDisplay();
}

void PlayerComponent::actionListenerCallback (const String& message) {
    if (message == Globals::ActionMessage::EnableTransport) // Just relay message to Transport
        sendActionMessage(message);
}
