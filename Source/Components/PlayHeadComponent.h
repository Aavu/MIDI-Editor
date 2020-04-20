//
// Created by Raghavasimhan Sankaranarayanan on 4/18/20.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlayerComponent.h"
#include "Globals.h"

class PlayHeadComponent : public juce::Component, public AudioPlayHead, public Timer {
public:
    PlayHeadComponent() = default;

    ~PlayHeadComponent() override {
        stopTimer();
    }

    void init(PlayerComponent* playerToAttach) {
        m_pPlayer = playerToAttach;
        m_playHead.setColour(TextButton::buttonColourId, Colours::black);
        m_playHead.onStateChange = [this] {handleStateChange();};
        addAndMakeVisible(m_playHead);
        startTimer (Globals::GUI::iUpdateInterval_ms);
    }

    void paint(Graphics& g) override {
        m_iTrackViewComponentWidth = m_screenArea.getWidth();

        m_playHead.setBounds(m_iCurrentPlayHeadPosition, m_screenArea.getY(), Globals::GUI::iPlayHeadWidth, m_screenArea.getHeight());
    }

    void resized() override {
        m_screenArea = getLocalBounds();
        m_playHead.setBounds(m_iCurrentPlayHeadPosition, m_screenArea.getY(), Globals::GUI::iPlayHeadWidth, m_screenArea.getHeight());
    }

private:
    void timerCallback() override {
        if (!m_pPlayer)
            return;
        int maxLength = m_pPlayer->getMaxBufferLength();
        if (maxLength > 0) {
            if (getCurrentPosition(m_currentPositionInfo)) {
                m_iCurrentPlayHeadPosition = m_currentPositionInfo.timeInSamples * m_screenArea.getWidth() * 1.0 / maxLength;
//                DBG(m_iCurrentPlayHeadPosition << "\t" << (int)m_currentPositionInfo.timeInSamples << "\t" << maxLength);
                repaint();
            }
        }
    }

    bool getCurrentPosition (CurrentPositionInfo& result) override {
        if (!m_pPlayer)
            return false;

        auto pos = m_pPlayer->getCurrentPosition();
        result.resetToDefault();
        result.isPlaying = (m_pPlayer->getPlayState() == PlayerComponent::PlayState::Playing);
        result.timeInSamples = pos;
        result.bpm = m_pPlayer->getBPM();
        result.timeInSeconds = pos/m_pPlayer->getSampleRate();
        result.timeSigNumerator = 4;
        result.timeSigDenominator = 4;
        result.editOriginTime = 0;
        result.isLooping = false;
        result.isRecording  = false;
        return true;
    }

    void handleStateChange() {
        if (m_playHead.getState() == juce::Button::ButtonState::buttonOver) {
            DBG("ButtonOver");
            setMouseCursor(MouseCursor::IBeamCursor);
        } else {
            DBG("Out");
            setMouseCursor(MouseCursor::NormalCursor);
        }
    }

    int m_iTrackViewComponentWidth = 0;
    int m_iCurrentPlayHeadPosition = 0;
    Rectangle<int> m_screenArea;

    PlayerComponent* m_pPlayer = nullptr;

    TextButton m_playHead;

    AudioPlayHead::CurrentPositionInfo m_currentPositionInfo{};
};
