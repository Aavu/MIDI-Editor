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

TrackViewComponent::TrackViewComponent()
{
}

void TrackViewComponent::init(int noOfTracks) {
    m_iNumTracks = noOfTracks;

//    m_tracks = new TextButton*[m_iNumTracks];

    m_header.setColour (TextButton::buttonColourId, Colours::cornflowerblue);
//    m_header.setButtonText ("Header");
    addAndMakeVisible (m_header);

    m_sidebar.setColour (TextButton::buttonColourId, Colours::grey);
//    m_sidebar.setButtonText ("Sidebar");
    addAndMakeVisible (m_sidebar);

    if (m_iNumTracks > 0) {
        for(int i=0; i<m_iNumTracks; i++) {
            m_tracks.push_back(new ScrollablePianoRollComponent());
            //m_tracks.at(i)->setColour (TextButton::buttonColourId, Colours::lime);
            addAndMakeVisible (m_tracks[i]);
            m_aiTrackHeight.push_back(k_iDefaultTrackHeight);
        }
    }
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
    auto headerHeight = 32;
    m_header.setBounds (area.removeFromTop(headerHeight));

    auto sidebarWidth = 128;
    m_sidebar.setBounds (area.removeFromLeft(sidebarWidth));

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
    addAndMakeVisible (m_tracks[m_iNumTracks]);
    m_aiTrackHeight.push_back(k_iDefaultTrackHeight);
    m_iNumTracks++;

    resized();
}
