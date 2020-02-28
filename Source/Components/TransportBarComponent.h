/*
  ==============================================================================

    TransportBarComponent.h
    Created: 11 Jan 2020 11:43:39am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlayerComponent.h"

//==============================================================================
/*
*/
class TransportBarComponent    : public Component
{
public:
    TransportBarComponent();
    ~TransportBarComponent();

    void paint (Graphics&) override;
    void resized() override;

    void init(PlayerComponent* playerComponent);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportBarComponent)
    TextButton playBtn;
    TextButton stopBtn;
    void playBtnClicked();
    void stopBtnClicked();

    PlayerComponent *player = nullptr;
};
