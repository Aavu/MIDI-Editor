/*
  ==============================================================================

    TrackViewComponent.cpp
    Created: 3 Mar 2020 8:32:54pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/
#include "TrackViewComponent.h"

int TrackParameters::m_iNumTracks = 0;
int TrackParameters::k_iDefaultTrackHeight = 300;
std::vector<int> TrackParameters::m_aiTrackHeight {};

TrackViewComponent::TrackViewComponent()
{
}

void TrackViewComponent::init(PlayerComponent* player) {
    // header (tool bar)
    m_header.setColour (TextButton::buttonColourId, Colours::cornflowerblue);
    addAndMakeVisible (m_header);
    
    // sidebar (mute/solo buttons for each track) (not implemented)
    m_sidebar.setColour (TextButton::buttonColourId, Colours::grey);
    //addAndMakeVisible (m_sidebar);
    
    // pianoroll list
    m_pianoRollListComp = new PianoRollListComponent();
    m_pianoRollListComp->init(player);

    addAndMakeVisible(*m_pianoRollListComp);
}

TrackViewComponent::~TrackViewComponent() {
    delete m_pianoRollListComp;
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

    m_iTrackViewComponentWidth = area.getWidth();
    m_pianoRollListComp->setBounds(area);
}

int TrackViewComponent::getNumTracks() const {
    return m_pianoRollListComp->getNumTracks();
}

void TrackViewComponent::addTrack(int numTimeStampsForPianoRoll) {
    m_pianoRollListComp->addTrack(numTimeStampsForPianoRoll);
    
    resized();
}

void TrackViewComponent::setTimeFormat(int timeFormat)
{
    // currently only allow time stamp ticks - see the documentation of Class MidiFile
    assert(timeFormat > 0);
    
    m_pianoRollListComp->setTimeFormat(timeFormat);
}

void TrackViewComponent::convertMidiMessageSequence(int trackIdx, const MidiMessageSequence *message)
{
    // pass to m_pianoRollListComp
    m_pianoRollListComp->convertMidiMessageSequence(trackIdx, message);
}
