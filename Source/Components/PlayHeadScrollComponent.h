/*
  ==============================================================================

    PlayHeadScrollComponent.h
    Created: 20 Apr 2020 12:22:25am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class PlayHeadScrollComponent : public Component {
public:
    PlayHeadScrollComponent() = default;

    void paint (Graphics& g) override {
        g.fillAll (Colours::darkgrey);
    }

    void resized() override {

    }

    std::function<void(int posX)> childClicked = nullptr;

private:
    void mouseUp (const MouseEvent& event) override {
        if (!childClicked)
            return;

        childClicked(event.getPosition().getX());
    }

    void mouseDrag (const MouseEvent& event) override {
        if (!childClicked)
            return;

        childClicked(event.getPosition().getX());
    }
};
