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

class TrackViewComponent : public TrackParameters, public Component {
public:
    TrackViewComponent();

    ~TrackViewComponent() override;

    void init(PlayerComponent* player);
    void paint(Graphics& g) override;
    void resized() override;

    int getNumTracks() const;

    void addTrack();

private:
    void handleClick();

    TextButton m_header;
    SidebarComponent m_sidebar;
    std::vector<TextButton*> m_tracks;

    PlayHeadScrollComponent m_playHeadScroll;
    std::shared_ptr<PlayHeadComponent> m_pPlayHead = nullptr;

//    int m_iNumTracks = 0;
//    std::vector<int> m_trackHeight;
//    const int k_iDefaultTrackHeight = 128;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackViewComponent)
};
