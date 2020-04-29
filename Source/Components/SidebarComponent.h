/*
  ==============================================================================

    SidebarComponent.h
    Created: 4 Mar 2020 1:02:40am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>

class SidebarComponent: public juce::Component {
public:
    SidebarComponent() {
    }

    void paint (Graphics& g) override {
        g.fillAll (Colours::grey);
    }

};