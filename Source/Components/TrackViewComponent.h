/*
  ==============================================================================

    TrackViewComponent.h
    Created: 3 Mar 2020 8:32:54pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "SidebarComponent.h"
#include "TrackParameters.h"
#include "PlayHeadComponent.h"
#include "PlayHeadScrollComponent.h"
#include "PlayerComponent.h"
#include "Globals.h"

#include "GUIComponent/ScrollablePianoRollComponent/ScrollablePianoRollComponent.h"
#include "GUIComponent/ScrollablePianoRollComponent/PianoRollNote.h"

class TrackViewComponent : public TrackParameters, public Component, public Timer {
public:
    TrackViewComponent();

    ~TrackViewComponent() override;

    void init(PlayerComponent* player);
    void paint(Graphics& g) override;
    void resized() override;

//    int getNumTracks() const;
//
//    void addTrack(int numTimeStampsForPianoRoll);

    void setTrack(int numTimeStampsForPianoRoll, const MidiMessageSequence *message);
    
    void setTimeFormat(int timeFormat);

private:
    void timerCallback() override;
    void updatePlayHeadPosition();
    void handleScrollCallback(int newPositionX);

    void convertMidiMessageSequence(int trackIdx, const MidiMessageSequence *message);

    TextButton m_header;
    SidebarComponent m_sidebar;
//    std::vector<ScrollablePianoRollComponent*> m_tracks;

    ScrollablePianoRollComponent* m_pTrack = nullptr;
    
    int m_iTimeFormat;

    PlayerComponent* m_pPlayer = nullptr;
    PlayHeadScrollComponent m_playHeadScroll;
    std::shared_ptr<PlayHeadComponent> m_pPlayHead = nullptr;

    long m_iMaxBufferLength = 0;
    long m_iCurrentPlayHeadPosition = 0;
    int m_iTrackViewComponentWidth = 0;

//    int m_iNumTracks = 0;
//    std::vector<int> m_trackHeight;
//    const int k_iDefaultTrackHeight = 128;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackViewComponent)
};
