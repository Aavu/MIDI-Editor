/*
  ==============================================================================

    PianoRollListListComponent.cpp
    Created: 27 Apr 2020 7:41:15pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#include "PianoRollListComponent.h"

PianoRollListComponent::PianoRollListComponent() : m_pPlayHead(std::make_shared<PlayHeadComponent>())
{
}

void PianoRollListComponent::init(PlayerComponent* player) {
    m_pPlayer = player;
    
    if (m_iNumTracks > 0) {
        for(int i=0; i<m_iNumTracks; i++) {
            m_tracks.push_back(new ScrollablePianoRollComponent());
            addAndMakeVisible (m_tracks[i]);
            m_aiTrackHeight.push_back(k_iDefaultTrackHeight);
        }
    }
    
    addAndMakeVisible(m_playHeadScroll);
    
    m_playHeadScroll.childClicked = [&](int data) {
        handleScrollCallback(data);
    };
    
    m_pPlayHead->init();
    
    addAndMakeVisible(*m_pPlayHead);
    
    m_pPlayHead->setVisible(false);
    
    startTimer (Globals::GUI::iUpdateInterval_ms);
    
}

PianoRollListComponent::~PianoRollListComponent() {
    stopTimer();
    for (int i=0; i<m_iNumTracks; i++) {
        delete m_tracks[i];
    }
}

void PianoRollListComponent::paint(Graphics &g) {
    g.fillAll (Colours::lightgrey);
}

void PianoRollListComponent::resized() {
    auto area = getLocalBounds();
    auto playHeadScrollHeight = Globals::GUI::iHeaderHeight;
    
    m_pPlayHead->setBounds(0, 0, area.getWidth(), area.getHeight());
    
    m_playHeadScroll.setBounds(area.removeFromTop(playHeadScrollHeight));
    
    m_iPianoRollListComponentWidth = area.getWidth();
    updatePlayHeadPosition();
    
    for(int i=0; i< m_iNumTracks; i++) {
        m_tracks.at(i)->setBounds(area);
    }
}

int PianoRollListComponent::getNumTracks() const {
    return m_iNumTracks;
}

void PianoRollListComponent::addTrack(int numTimeStampsForPianoRoll) {
    m_tracks.push_back(new ScrollablePianoRollComponent(numTimeStampsForPianoRoll));
    addAndMakeVisible (m_tracks[m_iNumTracks], 0);
    m_aiTrackHeight.push_back(k_iDefaultTrackHeight);
    m_iNumTracks++;
    
    m_pPlayHead->setVisible(true);
    
    resized();
}

void PianoRollListComponent::timerCallback() {
    if (!m_pPlayer)
        return;
    //m_iMaxBufferLength = m_pPlayer->getMaxBufferLength();
//    if (m_iMaxBufferLength > 0) {
//        m_iCurrentPlayHeadPosition = m_pPlayer->getCurrentPosition() * m_iPianoRollListComponentWidth / m_iMaxBufferLength;
//        //        DBG(m_iCurrentPlayHeadPosition << "\t" << m_pPlayer->getCurrentPosition() << "\t" << m_iTrackViewComponentWidth << "\t" << m_iMaxBufferLength);
//        updatePlayHeadPosition();
//    }
    m_iCurrentPlayHeadPosition = m_pPlayer->getCurrentPositionInQuarterNotes()*40;
    updatePlayHeadPosition();
}

void PianoRollListComponent::updatePlayHeadPosition() {
    auto area = getLocalBounds();
    auto playHeadPosition = Globals::PianoRoll::keyboardWidth + m_iCurrentPlayHeadPosition; // set playhead position here
    m_pPlayHead->setBounds((int)playHeadPosition, Globals::GUI::iHeaderHeight, Globals::GUI::iPlayHeadWidth, area.getHeight());
}

void PianoRollListComponent::handleScrollCallback(int newPositionX) {
    if (newPositionX < 0)
        return;
    auto value = (newPositionX * m_iMaxBufferLength) / m_iPianoRollListComponentWidth;
    m_pPlayer->allNotesOff();
    m_pPlayer->setCurrentPosition(value);
}

void PianoRollListComponent::setTimeFormat(int timeFormat)
{
    m_iTimeFormat = timeFormat;
    
    DBG(m_iTimeFormat);
    
    // currently only allow time stamp ticks - see the documentation of Class MidiFile
    assert(m_iTimeFormat > 0);
}

void PianoRollListComponent::convertMidiMessageSequence(int trackIdx, const MidiMessageSequence *message)
{
    // convert message to the format that the pianoroll component uses
    auto numEvents = message->getNumEvents();
    MidiMessageSequence::MidiEventHolder* const * eventHolder = message->begin();
    MidiMessage msg;
    
    for (int i = 0; i < numEvents; i++) {
        msg = eventHolder[i]->message;
        // convert to Class PianoRollNote and send to NoteLayer
        if (msg.isNoteOn()) {
            double timeStamp = msg.getTimeStamp();
            double timeStampNoteOff = message->getTimeOfMatchingKeyUp(i);
            int noteNumber = msg.getNoteNumber();
            uint8 noteVelocity = msg.getVelocity();
            
            //DBG(String(timeStamp/m_iTimeFormat) + "\t" + String((timeStampNoteOff-timeStamp+1)/m_iTimeFormat) + "\t" + String(noteNumber));
            PianoRollNote *newNote = new PianoRollNote(Globals::PianoRoll::midiNoteNum-1-noteNumber, timeStamp/m_iTimeFormat, (timeStampNoteOff-timeStamp+1)/m_iTimeFormat, noteVelocity);
            m_tracks.at(trackIdx)->addNote(newNote);
        }
    }
}
