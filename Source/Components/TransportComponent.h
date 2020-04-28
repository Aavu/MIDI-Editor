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

    struct SMPTE {
        int hh = 0, mm = 0, ss = 0, ff = 0;
    };

    void updateTempoDisplay(double bpm);

private:
    void playBtnClicked();
    void stopBtnClicked();

    void actionListenerCallback (const String& message) override;

    void updateTimeDisplay();
    void convertToSMPTE(SMPTE& smpte, long iPositionInSamples);

    TextButton m_playBtn;
    TextButton m_stopBtn;

    Label m_timeDisplay;
    Label m_timeLabel;

    Label m_bpmDisplay;
    Label m_bpmLabel;

    PlayerComponent *m_pPlayer = nullptr;

    unsigned int m_bpm = 120;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportComponent)
};
