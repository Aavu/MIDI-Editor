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
        const Image playBtnNormal  = ImageCache::getFromFile(File(
                CUtil::getResourcePath() + "/icons/playBtnNormal.png"));
        const Image playBtnDown    = ImageCache::getFromFile(File(
                CUtil::getResourcePath() + "/icons/playBtnDown.png"));
        const Image pauseBtnNormal = ImageCache::getFromFile(File(
                CUtil::getResourcePath() + "/icons/pauseBtnNormal.png"));
        const Image pauseBtnDown   = ImageCache::getFromFile(File(
                CUtil::getResourcePath() + "/icons/pauseBtnDown.png"));
        const Image stopBtnNormal  = ImageCache::getFromFile(File(
                CUtil::getResourcePath() + "/icons/stopBtnNormal.png"));
        const Image stopBtnDown    = ImageCache::getFromFile(File(
                CUtil::getResourcePath() + "/icons/stopBtnDown.png"));
    };

    Icons m_icons;

    ImageButton m_playBtn;
    ImageButton m_stopBtn;

    Label m_timeDisplay;
    Label m_timeLabel;

    Label m_bpmDisplay;
    Label m_bpmLabel;

    std::shared_ptr<PlayerComponent> m_pPlayer = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportComponent)
};
