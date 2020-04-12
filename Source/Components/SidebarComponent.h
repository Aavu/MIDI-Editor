/*
  ==============================================================================

    SidebarComponent.h
    Created: 4 Mar 2020 1:02:40am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "TrackParameters.h"

class SidebarComponent: public TrackParameters, public juce::Component {
public:
    SidebarComponent() {
        m_iNumTracks = 0;
    }

    void paint (Graphics& g) override {
        g.fillAll (Colours::grey);
    }
//
//    void resized() override {
//
//    }

};