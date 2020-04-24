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

    startTimer (Globals::GUI::iUpdateInterval_ms);

}

TrackViewComponent::~TrackViewComponent() {
    stopTimer();
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

    m_playHeadScroll.setBounds(sidebarWidth, headerHeight, area.getWidth(), headerHeight);

    m_iTrackViewComponentWidth = area.getWidth() - sidebarWidth;
    updatePlayHeadPosition();

    for(int i=0; i< m_iNumTracks; i++) {
        m_tracks.at(i)->setBounds(area.removeFromTop(m_aiTrackHeight.at(i)));
    }
}

int TrackViewComponent::getNumTracks() const {
    return m_iNumTracks;
}

void TrackViewComponent::addTrack() {
    m_tracks.push_back(new TextButton());
    m_tracks.at(m_iNumTracks)->setColour (TextButton::buttonColourId, Colours::lime);
    addAndMakeVisible (m_tracks[m_iNumTracks], 0);
    m_aiTrackHeight.push_back(k_iDefaultTrackHeight);
    m_iNumTracks++;

    resized();
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
    auto playHeadPosition = Globals::GUI::iSideBarWidth + m_iCurrentPlayHeadPosition;
    m_pPlayHead->setBounds((int)playHeadPosition, Globals::GUI::iHeaderHeight, Globals::GUI::iPlayHeadWidth, area.getHeight());
}

void TrackViewComponent::handleScrollCallback(int newPositionX) {
    if (newPositionX < 0)
        return;
    auto value = (newPositionX * m_iMaxBufferLength) / m_iTrackViewComponentWidth;
    m_pPlayer->allNotesOff();
    m_pPlayer->setCurrentPosition(value);
}
