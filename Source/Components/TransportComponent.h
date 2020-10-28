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
#include "Util.h"
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

    void init(std::shared_ptr<PlayerComponent> playerComponent);

    struct SMPTE {
        int hh = 0, mm = 0, ss = 0, ff = 0;
    };

    void updateDisplay();
private:
    void playBtnClicked();
    void stopBtnClicked();

    void actionListenerCallback (const String& message) override;

    void initDisplayComponents();
    void convertToSMPTE(SMPTE& smpte, long iPositionInSamples);

    void updateTempoDisplay(double bpm);
    void updateTimeDisplay(const SMPTE& smpte);

    struct Icons {
        const Image playBtnNormal  = ImageCache::getFromMemory(
                BinaryData::playBtnNormal_png, BinaryData::playBtnNormal_pngSize);
        const Image playBtnDown    = ImageCache::getFromMemory(
                BinaryData::playBtnDown_png, BinaryData::playBtnDown_pngSize);
        const Image pauseBtnNormal = ImageCache::getFromMemory(
                BinaryData::pauseBtnNormal_png, BinaryData::pauseBtnNormal_pngSize);
        const Image pauseBtnDown   = ImageCache::getFromMemory(
                BinaryData::pauseBtnDown_png, BinaryData::pauseBtnDown_pngSize);
        const Image stopBtnNormal  = ImageCache::getFromMemory(
                BinaryData::stopBtnNormal_png, BinaryData::stopBtnNormal_pngSize);
        const Image stopBtnDown    = ImageCache::getFromMemory(
                BinaryData::stopBtnDown_png, BinaryData::stopBtnDown_pngSize);
    };

    Icons m_icons;

    ImageButton m_playBtn;
    ImageButton m_stopBtn;

    Label m_timeDisplay;
    Label m_timeLabel;

    Label m_bpmDisplay;
    Label m_bpmLabel;

    std::shared_ptr<PlayerComponent> m_pPlayer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportComponent)
};
