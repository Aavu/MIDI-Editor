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
            sound->setChannelNum(i);
            m_synth.addSound(sound);
        }
        std::cout << sounds.size() << " sounds added." << std::endl;
    };
    m_sfzLoader.loadSounds(kiNumChannels, true, &addLoadedSoundCallback);

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
        addMessageToBuffer(msg);
    }

    m_pIterator = std::make_unique<MidiBuffer::Iterator>(m_midiBuffer);
    m_iMaxBufferLength = m_midiBuffer.getLastEventTime();
    DBG("max length : " << m_iMaxBufferLength);
}

MidiMessageSequence& PlayerComponent::getTempoEvents()
{
    return m_TempoEvents;
}

MidiMessageSequence& PlayerComponent::getTempoEventsInSecs()
{
    return m_TempoEventsInSec;
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
    double newPositionInSamples = 0;
    if (m_TempoEvents.getNumEvents() > 0)
    {
        double target_tick = newPositionInQuarterNotes * m_iTimeFormat;
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
        newPositionInSamples = st_sec * m_fSampleRate + (target_tick-st_tick) * m_fSampleRate * cur_tempo / m_iTimeFormat;
    }
    m_iCurrentPosition = static_cast<long>(newPositionInSamples);
    return;
}

void PlayerComponent::setTimeFormat(int timeFormat)
{
    m_iTimeFormat = timeFormat;
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

String PlayerComponent::getAbsolutePathOfProject(const String &projectFolderName) {
    File currentDir = File::getCurrentWorkingDirectory();

    while (currentDir.getFileName() != projectFolderName) {
        currentDir = currentDir.getParentDirectory();
        if (currentDir.getFullPathName() == "/")
            return String();
    }
    return currentDir.getFullPathName();
}
