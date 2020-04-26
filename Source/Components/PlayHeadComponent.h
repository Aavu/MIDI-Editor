//
// Created by Raghavasimhan Sankaranarayanan on 4/18/20.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlayerComponent.h"
#include "Globals.h"

class PlayHeadComponent : public Component {
public:
    void init() {
        m_playHead.setColour(TextButton::buttonColourId, Colours::black);
        addAndMakeVisible(m_playHead);
    }

    void paint(Graphics& g) override {
    }

    void resized() override {
        auto screenArea = getLocalBounds();
        m_playHead.setBounds(0, screenArea.getY(), screenArea.getWidth(), screenArea.getHeight());
    }

private:
    TextButton m_playHead;
};
