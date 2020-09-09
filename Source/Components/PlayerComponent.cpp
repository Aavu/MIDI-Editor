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

std::shared_ptr<PlayerComponent> PlayerComponent::getInstance() {
    static std::shared_ptr<PlayerComponent> m_pInstance(new PlayerComponent());
    return m_pInstance;
}

PlayerComponent::PlayerComponent()
{
    initSynth();
    startTimer(Globals::GUI::iUpdateInterval_ms);
}

PlayerComponent::~PlayerComponent()
{
    stopTimer();
    delete m_midiMessageSequence;
}

void PlayerComponent::paint (Graphics& g)
{
}

void PlayerComponent::resized()
{
}

void PlayerComponent::clearTempoEvents()
{
    m_TempoEvents.clear();
    m_TempoEventsInSec.clear();
}

void PlayerComponent::initSynth() {
    File * soundFontFile = new File(getAbsolutePathOfProject() + "/Resources/SoundFonts/GeneralUser GS 1.442 MuseScore/GeneralUser GS MuseScore v1.442.sf2");
    //File * soundFontFile = new File(CUtil::getResourcePath() + "/SoundFonts/GeneralUser GS 1.442 MuseScore/GeneralUser GS MuseScore v1.442.sf2");
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
    m_pTempoIterator = std::make_unique<MidiBuffer::Iterator>(m_tempoEventBuffer);
}

void PlayerComponent::fillMidiBuffer(int iNumSamples) {
    DBG("----PlayerComponent::fillMidiBuffer-----------------");
    // Retrieve messages upto block end position (sample num)
    auto iBlockEndPosition = m_iCurrentPosition + iNumSamples;

    while ((m_iLastRetrievedPosition < iBlockEndPosition) && (m_iMidiEventReadIdx < m_iMaxMidiEvents)) {
            auto * pEventAtReadIdx = m_midiMessageSequence->getEventPointer(m_iMidiEventReadIdx);
            auto msg = pEventAtReadIdx->message;
            auto msgSampleNum = static_cast<long> (msg.getTimeStamp() * m_fSampleRate);
            if (msgSampleNum > iBlockEndPosition) {
                DBG("Msg not retrieved");
                break;
            }
            DBG("--Retrieved-msg------------------");
            DBG("At ReadIdx: " << m_iMidiEventReadIdx);
            DBG("Msg sample num: " << msgSampleNum);
            m_midiBuffer.addEvent(msg, static_cast<int>(msgSampleNum));  //TODO: Might be able to get rid of m_midiBuffer and directly use m_currentMidiBuffer.
            m_iLastRetrievedPosition = msgSampleNum;
            m_iMidiEventReadIdx++;
            DBG("--------------------------------");
    }
    DBG("m_iCurrentPosition: " << m_iCurrentPosition << "\n" << "m_iLastRetrievedPosition: " << m_iLastRetrievedPosition);
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
    return convertSecToQuarterNote(m_iCurrentPosition*1.F/m_fSampleRate);
}

void PlayerComponent::setCurrentPositionByQuarterNotes(double newPositionInQuarterNotes)
{
    double newPositionInSamples = convertQuarterNoteToSec(newPositionInQuarterNotes) * m_fSampleRate;
    m_iCurrentPosition = static_cast<long> (newPositionInSamples);
}

void PlayerComponent::setTimeFormat(int timeFormat)
{
    m_iTimeFormat = timeFormat;
}

void PlayerComponent::resetPlayer() {
    m_iMidiEventReadIdx = 0;
    m_iCurrentPosition = 0;
    m_iLastRetrievedPosition = 0;
    m_midiBuffer.clear();
}

MidiMessageSequence *PlayerComponent::getMidiMessageSequence() const {
    if (m_midiMessageSequence)
        return m_midiMessageSequence;
    return nullptr;
}

void PlayerComponent::setMidiMessageSequence(MidiMessageSequence* midiMsgSeq) {
    sendActionMessage(Globals::ActionMessage::Stop);
    m_midiMessageSequence = midiMsgSeq;
    m_midiMessageSequence->sort();

    resetCurrentPosition();
    m_iMaxMidiEvents = m_midiMessageSequence->getNumEvents();
    m_iMaxBufferLength = static_cast<long>((m_midiMessageSequence->getEndTime() + m_fMinBufferLengthInSec) * m_fSampleRate);
    resetPlayer();

    m_midiBuffer.clear();
    m_iMaxBufferLength = static_cast<long>(m_midiMessageSequence->getEndTime() * m_fSampleRate);
    // TODO: Merge Fuckup
    addAllTempoMessagesToBuffer();

    if (midiMsgSeq != nullptr) {
        sendActionMessage(Globals::ActionMessage::EnableTransport);
        m_bSequenceLoaded = true;
    }
}

void PlayerComponent::play() {
    m_playState = PlayState::Playing;
}

void PlayerComponent::pause() {
    m_playState = PlayState::Paused;
}

void PlayerComponent::stop() {
    m_playState = PlayState::Stopped;
    DBG("Stop");
    resetPlayer();
    //TODO: Merge Fuckup
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
            DBG("----Need-more-samples-------------------------");
            DBG("m_iCurrentPosition: " << m_iCurrentPosition << "\n" << "m_iLastRetrievedPosition: " << m_iLastRetrievedPosition << "\n" << "Need msgs upto sample num: "<< (m_iCurrentPosition+blockSize) - m_iLastRetrievedPosition);
            fillMidiBuffer(blockSize);
        } else {
//            DBG("-----" << (m_iCurrentPosition + blockSize) << "-- " << m_iLastRetrievedPosition);
        }

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

PlayerComponent::PlayState PlayerComponent::getPlayState() const {return m_playState;}

double PlayerComponent::getSampleRate() const {return m_fSampleRate;}

long PlayerComponent::getMaxBufferLength() const {return m_iMaxBufferLength;}

long PlayerComponent::getCurrentPosition() const {return m_iCurrentPosition;}

void PlayerComponent::updateTempo() {
    if (!m_pTempoIterator)
        return;
    m_pTempoIterator->setNextSamplePosition((int) m_iCurrentPosition);
    MidiMessage result;
    int samplePosition;
    m_pTempoIterator->getNextEvent(result, samplePosition);
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
    //TODO:Merge Fuckup
}

void PlayerComponent::updateNote(int iNoteOnEventIndex, double fNewNoteOnTimestampInQuarterNotes, double fNoteDurationInQuarterNotes /*= -1*/, int iNewNoteNumber /*= -1*/) {
    DBG("-------------updateNoteTimestamps--------------------");
    const ScopedLock scopedLock(m_criticalSection);

    auto * pEventAtReadIdx = m_midiMessageSequence->getEventPointer(m_iMidiEventReadIdx); // To maintain read index after sort
    if (pEventAtReadIdx)
        DBG("Old EventAtReadIndex: " << pEventAtReadIdx->message.getDescription() << " " <<pEventAtReadIdx->message.getTimeStamp());

    // Get noteOn and noteOff events
    auto * pNoteOnEvent = m_midiMessageSequence->getEventPointer(iNoteOnEventIndex);
    auto * pNoteOffEvent = m_midiMessageSequence->getEventPointer(m_midiMessageSequence->getIndexOfMatchingKeyUp(iNoteOnEventIndex));
    DBG( "Old Note On: " << pNoteOnEvent->message.getDescription() << " " << pNoteOnEvent->message.getTimeStamp());
    DBG( "Old Note Off: " << pNoteOffEvent->message.getDescription() << " " << pNoteOffEvent->message.getTimeStamp());
    DBG( "Old Note Length: " << pNoteOffEvent->message.getTimeStamp() - pNoteOnEvent->message.getTimeStamp());

    // Update timestamps for both noteOn and noteOff
    auto fNewNoteOnTimestamp = convertQuarterNoteToSec(fNewNoteOnTimestampInQuarterNotes);
    double fNoteDuration;
    if (fNoteDurationInQuarterNotes == -1) // keep duration same as before
        fNoteDuration = pNoteOffEvent->message.getTimeStamp() - pNoteOnEvent->message.getTimeStamp();
    else
        fNoteDuration = convertQuarterNoteToSec(fNoteDurationInQuarterNotes);
    pNoteOnEvent->message.setTimeStamp(fNewNoteOnTimestamp);
    pNoteOffEvent->message.setTimeStamp(fNewNoteOnTimestamp + fNoteDuration);

    // Update pitch of both noteOn abd noteOff events
    if (iNewNoteNumber != -1) {
        pNoteOnEvent->message.setNoteNumber(iNewNoteNumber);
        pNoteOffEvent->message.setNoteNumber(iNewNoteNumber);
    }

    // Re-order MidiMessageSequence
    m_midiMessageSequence->sort();
    m_midiMessageSequence->updateMatchedPairs();

    // Set read index back to correct position after re-ordering.
    if (pEventAtReadIdx)
        m_iMidiEventReadIdx = m_midiMessageSequence->getIndexOf(pEventAtReadIdx);
    if (m_iMidiEventReadIdx == -1)
        m_iMidiEventReadIdx = m_iMaxMidiEvents;

    if (pEventAtReadIdx)
        DBG("New EventAtReadIndex: " << pEventAtReadIdx->message.getDescription() << " " <<pEventAtReadIdx->message.getTimeStamp());

    DBG( "New Note On: " << pNoteOnEvent->message.getDescription() << " " << pNoteOnEvent->message.getTimeStamp());
    DBG( "New Note Off: " << pNoteOffEvent->message.getDescription() << " " << pNoteOffEvent->message.getTimeStamp());
    DBG( "New Note Length: " << pNoteOffEvent->message.getTimeStamp() - pNoteOnEvent->message.getTimeStamp());
    // Reset buffer length if length of midi has changed (i.e. if last note moved)
    m_iMaxBufferLength = static_cast<long>((m_midiMessageSequence->getEndTime() + m_fMinBufferLengthInSec) * m_fSampleRate);

    DBG("----------------------------------------------------");
}

void PlayerComponent::addNote(PianoRollNote * pPianoRollNote) {
    // Create midi messages
    auto noteOnMsg = MidiMessage::noteOn (1, pPianoRollNote->getNoteNumber(), (uint8) 120);
    double noteOnTime = convertQuarterNoteToSec(pPianoRollNote->getOffset());
    noteOnMsg.setTimeStamp(noteOnTime);

    auto noteOffMsg = MidiMessage::noteOff (1, pPianoRollNote->getNoteNumber(), (uint8) 120);
    double noteOffTime = convertQuarterNoteToSec(pPianoRollNote->getOffset() + pPianoRollNote->getLength());
    noteOffMsg.setTimeStamp(noteOffTime);

    // Add to midiMessageSequence
    auto * e1 = m_midiMessageSequence->addEvent(noteOnMsg);
    pPianoRollNote->setNoteOnEventPtr(e1);
    auto * e2 = m_midiMessageSequence->addEvent(noteOffMsg);
    pPianoRollNote->setNoteOffEventPtr(e2);
    e1->noteOffObject = e2;

    m_midiMessageSequence->updateMatchedPairs();
    m_iMaxMidiEvents = m_midiMessageSequence->getNumEvents();
}

void PlayerComponent::deleteNote(int iNoteOnEventIndex) {
    m_midiMessageSequence->deleteEvent(iNoteOnEventIndex, true);
    m_midiMessageSequence->updateMatchedPairs();
    m_iMaxMidiEvents = m_midiMessageSequence->getNumEvents();
}


void PlayerComponent::timerCallback() {
    updateTransportDisplay();
}

void PlayerComponent::actionListenerCallback (const String& message) {
    if (message == Globals::ActionMessage::EnableTransport) // Just relay message to Transport
        sendActionMessage(message);
}

bool PlayerComponent::isSequenceLoaded() {
    return m_bSequenceLoaded;
}

double PlayerComponent::getTempo() {
    updateTempo();
    return m_fCurrentTempo;
}

// TODO: Remove later. Use function from CUtil instead.
String PlayerComponent::getAbsolutePathOfProject(const String &projectFolderName) {
    File currentDir = File::getCurrentWorkingDirectory();

    while (currentDir.getFileName() != projectFolderName) {
        currentDir = currentDir.getParentDirectory();
        if (currentDir.getFullPathName() == "/")
            return String();
    }
    return currentDir.getFullPathName();
}