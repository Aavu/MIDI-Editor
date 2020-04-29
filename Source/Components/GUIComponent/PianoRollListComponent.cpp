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
    
    
    addAndMakeVisible(m_playHeadScroll);
    
    m_playHeadScroll.m_TimeAxis.childClicked = [&](int data) {
        handleScrollCallback(data);
    };
    
    m_pPlayHead->init();
    
    addAndMakeVisible(*m_pPlayHead);
    
    m_pPlayHead->setVisible(false);
    m_playHeadScroll.setVisible(false);
    
}

PianoRollListComponent::~PianoRollListComponent() {
    stopTimer();
    for (int i=0; i< m_iNumTracks; i++) {
        delete m_tracks[i];
    }
}
//
//void PianoRollListComponent::paint(Graphics &g) {
//    g.fillAll (Colours::lightgrey);
//}

void PianoRollListComponent::resized() {
    auto area = getLocalBounds();
    auto playHeadScrollHeight = Globals::GUI::iHeaderHeight;
    
    m_pPlayHead->setBounds(0, 0, area.getWidth(), area.getHeight());
    
    m_playHeadScroll.setBounds(Globals::PianoRoll::keyboardWidth, 0, area.getWidth()-Globals::PianoRoll::keyboardWidth, playHeadScrollHeight);
    
    area.removeFromTop(playHeadScrollHeight);
    
    m_iPianoRollListComponentWidth = area.getWidth();
    updatePlayHeadPosition();
    
    for(int i=0; i< m_iNumTracks; i++) {
        m_tracks.at(i)->setBounds(area);
    }
}

int PianoRollListComponent::getNumTracks() const {
    return m_iNumTracks;
}

void PianoRollListComponent::setTrack(int numTimeStampsForPianoRoll) {
    if (m_iNumTracks == 1){
        m_iNumTracks = 0;
        delete m_tracks[0];
        m_tracks.pop_back();
    }
    m_tracks.push_back(new ScrollablePianoRollComponent(numTimeStampsForPianoRoll));
    m_tracks[0]->m_syncScrollBars = [this] (int setViewPosition) { syncViewPositionX(setViewPosition); };
    addAndMakeVisible (m_tracks[m_iNumTracks], 0);

    m_iNumTracks = 1;
    
    m_playHeadScroll.m_syncScrollBars = [this] (int setViewPosition) { syncViewPositionX(setViewPosition); };
    m_playHeadScroll.setBoxWidthAndNumBox(m_tracks[0]->getBoxWidth(), numTimeStampsForPianoRoll);
    m_playHeadScroll.setSize(getWidth(), Globals::GUI::iHeaderHeight);
    
    m_pPlayHead->setVisible(true);
    m_playHeadScroll.setVisible(true);
    
    resized();
    
    startTimer (Globals::GUI::iUpdateInterval_ms);
}

void PianoRollListComponent::timerCallback() {
    if (!m_pPlayer || m_iNumTracks == 0)
        return;
    m_iCurrentPlayHeadPosition = m_pPlayer->getCurrentPositionInQuarterNotes()*m_tracks.at(0)->getBoxWidth() + m_tracks.at(0)->getViewPositionX();
    if (m_iCurrentPlayHeadPosition < 0)
        m_iCurrentPlayHeadPosition = -Globals::PianoRoll::keyboardWidth-10;
    updatePlayHeadPosition();
    m_pPlayer->updateTransportDisplay();
}

void PianoRollListComponent::updatePlayHeadPosition() {
    auto area = getLocalBounds();
    auto playHeadPosition = Globals::PianoRoll::keyboardWidth + m_iCurrentPlayHeadPosition; // set playhead position here
    m_pPlayHead->setBounds((int)playHeadPosition, Globals::GUI::iHeaderHeight, Globals::GUI::iPlayHeadWidth, area.getHeight());
}

void PianoRollListComponent::handleScrollCallback(int newPositionX) {
    if (newPositionX < 0)
        return;
    m_pPlayer->allNotesOff();
    if (getNumTracks() > 0)
    {
        double positionByQuarterNote = newPositionX*1.F / m_tracks[0]->getBoxWidth();
        m_pPlayer->setCurrentPositionByQuarterNotes(positionByQuarterNote);
    }
}

void PianoRollListComponent::setTimeFormat(int timeFormat)
{
    m_iTimeFormat = timeFormat;
    
    // DBG(m_iTimeFormat);
    
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
            int idxNoteOff = message->getIndexOfMatchingKeyUp(i);
            double timeStampNoteOff = message->getEventTime(idxNoteOff);
            int noteNumber = msg.getNoteNumber();
            uint8 noteVelocity = msg.getVelocity();
            
            //DBG(String(timeStamp/m_iTimeFormat) + "\t" + String((timeStampNoteOff-timeStamp+1)/m_iTimeFormat) + "\t" + String(noteNumber));
            
            PianoRollNote *newNote = new PianoRollNote(Globals::PianoRoll::midiNoteNum-1-noteNumber, timeStamp/m_iTimeFormat, (timeStampNoteOff-timeStamp+1)/m_iTimeFormat, noteVelocity);
            
            m_tracks.at(trackIdx)->addNote(newNote);
        }
    }
}

void PianoRollListComponent::syncViewPositionX(int setViewPosition)
{
    for (int i = 0; i < m_iNumTracks; i++)
        m_tracks[i]->setViewPositionX(setViewPosition);
    m_playHeadScroll.setViewPositionX(setViewPosition);
}
