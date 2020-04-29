/*
  ==============================================================================

    TrackViewComponent.cpp
    Created: 3 Mar 2020 8:32:54pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/
#include "TrackViewComponent.h"

//int TrackParameters::m_iNumTracks = 0;
//int TrackParameters::k_iDefaultTrackHeight = 300;
//std::vector<int> TrackParameters::m_aiTrackHeight {};

TrackViewComponent::TrackViewComponent() : m_pPlayHead(std::make_shared<PlayHeadComponent>())
{
}

void TrackViewComponent::init(PlayerComponent* player) {
    m_pPlayer = player;
    m_header.setColour (TextButton::buttonColourId, Colours::cornflowerblue);
    addAndMakeVisible (m_header);

    m_sidebar.setColour (TextButton::buttonColourId, Colours::grey);
    addAndMakeVisible (m_sidebar);

    addAndMakeVisible(m_playHeadScroll);

    m_playHeadScroll.childClicked = [&](int data) {
        handleScrollCallback(data);
    };

    m_pPlayHead->init();

    addAndMakeVisible(*m_pPlayHead);
    
    m_pPlayHead->setVisible(false);

    startTimer (Globals::GUI::iUpdateInterval_ms);

}

TrackViewComponent::~TrackViewComponent() {
    stopTimer();
    delete m_pTrack;
}

void TrackViewComponent::paint(Graphics &g) {
    g.fillAll (Colours::lightgrey);
}

void TrackViewComponent::resized() {
    auto area = getLocalBounds();
    auto headerHeight = Globals::GUI::iHeaderHeight;
    m_header.setBounds (area.removeFromTop(headerHeight));

    auto sidebarWidth = Globals::GUI::iSideBarWidth;
    m_sidebar.setBounds (area.removeFromLeft(sidebarWidth));
    
    m_pPlayHead->setBounds(sidebarWidth, headerHeight, area.getWidth(), area.getHeight());

    m_playHeadScroll.setBounds(area.removeFromTop(headerHeight));

    m_iTrackViewComponentWidth = area.getWidth() - sidebarWidth;
    updatePlayHeadPosition();

    if (m_pTrack)
        m_pTrack->setBounds(area);
}

void TrackViewComponent::setTrack(int numTimeStampsForPianoRoll, const MidiMessageSequence *message) {
    delete m_pTrack;
    m_pTrack = new ScrollablePianoRollComponent(numTimeStampsForPianoRoll);
    addAndMakeVisible (m_pTrack, 0);
    m_pPlayHead->setVisible(true);
    resized();

    convertMidiMessageSequence(0, message);
}

void TrackViewComponent::timerCallback() {
    if (!m_pPlayer)
        return;
    m_iMaxBufferLength = m_pPlayer->getMaxBufferLength();
    if (m_iMaxBufferLength > 0) {
        m_iCurrentPlayHeadPosition = m_pPlayer->getCurrentPosition() * m_iTrackViewComponentWidth / m_iMaxBufferLength;
//        DBG(m_iCurrentPlayHeadPosition << "\t" << m_pPlayer->getCurrentPosition() << "\t" << m_iTrackViewComponentWidth << "\t" << m_iMaxBufferLength);
        updatePlayHeadPosition();
    }
}

void TrackViewComponent::updatePlayHeadPosition() {
    auto area = getLocalBounds();
    auto playHeadPosition = Globals::GUI::iSideBarWidth + Globals::PianoRoll::keyboardWidth + m_iCurrentPlayHeadPosition;
    m_pPlayHead->setBounds((int)playHeadPosition, Globals::GUI::iHeaderHeight, Globals::GUI::iPlayHeadWidth, area.getHeight());
}

void TrackViewComponent::handleScrollCallback(int newPositionX) {
    if (newPositionX < 0)
        return;
    auto value = (newPositionX * m_iMaxBufferLength) / m_iTrackViewComponentWidth;
    m_pPlayer->allNotesOff();
    m_pPlayer->setCurrentPosition(value);
}

void TrackViewComponent::setTimeFormat(int timeFormat)
{
    m_iTimeFormat = timeFormat;
    
//    DBG(m_iTimeFormat);
    
    // currently only allow time stamp ticks - see the documentation of Class MidiFile
    assert(m_iTimeFormat > 0);
}

void TrackViewComponent::convertMidiMessageSequence(int trackIdx, const MidiMessageSequence *message)
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
            m_pTrack[0].addNote(newNote);
        }
    }
}
