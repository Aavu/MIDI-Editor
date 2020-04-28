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
        auto area = getLocalBounds();
        m_playHead.setBounds(area.removeFromLeft(Globals::PianoRoll::keyboardWidth));
    }

private:
    TextButton m_playHead;
};
