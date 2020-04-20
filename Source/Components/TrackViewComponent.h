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

#include "GUIComponent/ScrollablePianoRollComponent/ScrollablePianoRollComponent.h"

class TrackViewComponent : public TrackParameters, public juce::Component {
public:
    TrackViewComponent();

    ~TrackViewComponent();

    void init(int noOfTracks);
    void paint(Graphics& g) override;
    void resized() override;

    int getNumTracks() const;

    void addTrack();

private:
    TextButton m_header;
    SidebarComponent m_sidebar;
    std::vector<ScrollablePianoRollComponent*> m_tracks;

//    int m_iNumTracks = 0;
//    std::vector<int> m_trackHeight;
//    const int k_iDefaultTrackHeight = 128;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackViewComponent)
};
