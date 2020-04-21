/*
  ==============================================================================

    TransportComponent.h
    Created: 11 Jan 2020 11:43:39am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlayerComponent.h"
#include "Globals.h"

//==============================================================================
/*
*/
class TransportComponent : public Component, public ActionListener
{
public:
    TransportComponent();
    ~TransportComponent() override;

    void paint (Graphics&) override;
    void resized() override;

    void init(PlayerComponent* playerComponent);

private:
    void playBtnClicked();
    void stopBtnClicked();

    void actionListenerCallback (const String& message) override;

    TextButton m_playBtn;
    TextButton m_stopBtn;
    PlayerComponent *m_pPlayer = nullptr;

    unsigned int bpm = 120;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportComponent)
};
