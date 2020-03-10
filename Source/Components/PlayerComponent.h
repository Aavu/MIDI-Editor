/*
  ==============================================================================

    PlayerComponent.h
    Created: 12 Jan 2020 8:47:56pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class PlayerComponent    : public Component
{
public:
    PlayerComponent();
    ~PlayerComponent();

    void paint (Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerComponent)
};
