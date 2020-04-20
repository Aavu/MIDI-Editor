/*
  ==============================================================================

    PlayHeadScrollComponent.h
    Created: 20 Apr 2020 12:22:25am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PlayHeadComponent.h"

class PlayHeadScrollComponent : public Component {
public:
    PlayHeadScrollComponent() {

    }

    void init(PlayHeadComponent* playHead) {
        m_playHead = playHead;
    }

    void paint (Graphics& g) override {
        g.fillAll (Colours::darkgrey);
    }

    void resized() override {

    }

private:
    void mouseUp (const MouseEvent& event) override {
        if (!m_playHead)
            return;

//        DBG("mouse up: " << event.getPosition().getX());
        m_playHead->handleScrollCallback(event.getPosition().getX());
    }

    PlayHeadComponent* m_playHead = nullptr;
};
