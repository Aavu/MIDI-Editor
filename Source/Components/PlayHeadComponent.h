//
// Created by Raghavasimhan Sankaranarayanan on 4/18/20.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PlayerComponent.h"
#include "Globals.h"

class PlayHeadComponent : public Component, public AudioPlayHead, public Timer {
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
        m_playHead.setBounds(m_iCurrentPlayHeadPosition, m_screenArea.getY(), Globals::GUI::iPlayHeadWidth, m_screenArea.getHeight());
    }

    void resized() override {
        m_screenArea = getLocalBounds();
        m_iTrackViewComponentWidth = m_screenArea.getWidth();
        m_playHead.setBounds(m_iCurrentPlayHeadPosition, m_screenArea.getY(), Globals::GUI::iPlayHeadWidth, m_screenArea.getHeight());
    }

    void handleScrollCallback(int newPositionX) {
        auto value = static_cast<int>((newPositionX * 1.0 / m_iTrackViewComponentWidth) * m_iMaxBufferLength);
        m_pPlayer->setCurrentPosition(value);
    }

private:
    void timerCallback() override {
        if (!m_pPlayer)
            return;
        m_iMaxBufferLength = m_pPlayer->getMaxBufferLength();
        if (m_iMaxBufferLength > 0) {
            if (getCurrentPosition(m_currentPositionInfo)) {
                m_iCurrentPlayHeadPosition = m_currentPositionInfo.timeInSamples * m_iTrackViewComponentWidth * 1.0 / m_iMaxBufferLength;
//                DBG(m_iCurrentPlayHeadPosition << "\t" << (int)m_currentPositionInfo.timeInSamples << "\t" << maxLength);
                repaint();
            }
        }
    }

    bool getCurrentPosition (CurrentPositionInfo& result) override {
        if (!m_pPlayer)
            return false;

        auto pos                    = m_pPlayer->getCurrentPosition();
        result.resetToDefault();
        result.isPlaying            = (m_pPlayer->getPlayState() == PlayerComponent::PlayState::Playing);
        result.timeInSamples        = pos;
        result.bpm                  = m_pPlayer->getBPM();
        result.timeInSeconds        = pos/m_pPlayer->getSampleRate();
        result.timeSigNumerator     = 4;
        result.timeSigDenominator   = 4;
        result.editOriginTime       = 0;
        result.isLooping            = false;
        result.isRecording          = false;
        return true;
    }

    void handleStateChange() {
        if (m_playHead.getState() == juce::Button::ButtonState::buttonOver) {
            setMouseCursor(MouseCursor::IBeamCursor);
        } else {
            setMouseCursor(MouseCursor::NormalCursor);
        }
    }

    int m_iTrackViewComponentWidth  = 0;
    int m_iCurrentPlayHeadPosition  = 0;
    int m_iMaxBufferLength          = 0;
    Rectangle<int> m_screenArea;

    PlayerComponent* m_pPlayer      = nullptr;

    TextButton m_playHead;

    AudioPlayHead::CurrentPositionInfo m_currentPositionInfo{};
};
