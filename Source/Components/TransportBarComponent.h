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
    ~TransportBarComponent() override;

    void paint (Graphics&) override;
    void resized() override;

    void init(PlayerComponent* playerComponent);

private:
    TextButton m_playBtn;
    TextButton m_stopBtn;
    void playBtnClicked();
    void stopBtnClicked();

    PlayerComponent *m_pPlayer = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportBarComponent)
};
