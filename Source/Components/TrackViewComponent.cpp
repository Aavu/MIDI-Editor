/*
  ==============================================================================

    TrackViewComponent.cpp
    Created: 3 Mar 2020 8:32:54pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/
#include "TrackViewComponent.h"

int TrackParameters::m_iNumTracks = 0;
int TrackParameters::k_iDefaultTrackHeight = 128;
std::vector<int> TrackParameters::m_aiTrackHeight {};

TrackViewComponent::TrackViewComponent() : m_pPlayHead(std::make_shared<PlayHeadComponent>())
{
}

void TrackViewComponent::init(PlayerComponent* player) {
    m_header.setColour (TextButton::buttonColourId, Colours::cornflowerblue);
    addAndMakeVisible (m_header);

    m_sidebar.setColour (TextButton::buttonColourId, Colours::grey);
    addAndMakeVisible (m_sidebar);

    if (m_iNumTracks > 0) {
        for(int i=0; i<m_iNumTracks; i++) {
            m_tracks.push_back(new ScrollablePianoRollComponent());
            //m_tracks.at(i)->setColour (TextButton::buttonColourId, Colours::lime);
            addAndMakeVisible (m_tracks[i]);
            m_aiTrackHeight.push_back(k_iDefaultTrackHeight);
        }
    }

    m_pPlayHead->init(player);
    addAndMakeVisible(*m_pPlayHead);

    m_playHeadScroll.init(m_pPlayHead.get());
    addAndMakeVisible(m_playHeadScroll);

}

TrackViewComponent::~TrackViewComponent() {
    for (int i=0; i<m_iNumTracks; i++) {
        delete m_tracks[i];
    }
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

    m_playHeadScroll.setBounds(sidebarWidth, headerHeight, area.getWidth(), headerHeight);

    for(int i=0; i< m_iNumTracks; i++) {
        m_tracks.at(i)->setBounds(area.removeFromTop(m_aiTrackHeight.at(i)));
    }
}

int TrackViewComponent::getNumTracks() const {
    return m_iNumTracks;
}

void TrackViewComponent::addTrack() {
    m_tracks.push_back(new ScrollablePianoRollComponent());
    m_tracks.at(m_iNumTracks)->setColour (TextButton::buttonColourId, Colours::lime);
    addAndMakeVisible (m_tracks[m_iNumTracks], 0);
    m_aiTrackHeight.push_back(k_iDefaultTrackHeight);
    m_iNumTracks++;

    resized();
}

void TrackViewComponent::handleClick() {
    float x = Desktop::getInstance().getMainMouseSource().getScreenPosition().getX();
    DBG(x);
}

void TrackViewComponent::setTimeFormat(int timeFormat)
{
    m_iTimeFormat = timeFormat;
    
    // only allow time stamp ticks - see the documentation of Class MidiFile
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
        if (msg.isNoteOnOrOff()) {
            // convert to Class PianoRollNote and send to NoteLayer
        }
    }
}
