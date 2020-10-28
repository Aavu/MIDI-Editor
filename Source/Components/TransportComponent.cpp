/*
  ==============================================================================

    TransportComponent.cpp
    Created: 11 Jan 2020 11:43:39am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TransportComponent.h"

#include <utility>

//==============================================================================
TransportComponent::TransportComponent():
        m_playBtn("play"),
        m_stopBtn("stop"),
        m_timeDisplay("Time Display", "00:00:00:00"),
        m_timeLabel("Time Label", "Time"),
        m_bpmDisplay("BPM Display", "120.0"),
        m_bpmLabel("BPM Label", "BPM"),
        m_pPlayer(nullptr)
{
    initDisplayComponents();

    addAndMakeVisible(m_playBtn);
    addAndMakeVisible(m_stopBtn);
    addAndMakeVisible(m_timeDisplay);
}

void TransportComponent::initDisplayComponents() {
    m_timeDisplay.setFont (Font (24.0f, Font::bold));
    m_timeDisplay.setText ("00:00:00:00", dontSendNotification);
    m_timeDisplay.setJustificationType (Justification::centred);
    m_timeDisplay.setColour (Label::backgroundColourId, Colours::black);
    m_timeDisplay.setColour (Label::textColourId, Colours::orange);
    m_timeDisplay.setColour (Label::outlineColourId, Colours::orange);
//    m_timeDisplay.setEditable(false, true);

    m_timeDisplay.onEditorShow = [this] {
        auto* editor = m_timeDisplay.getCurrentTextEditor();
        editor->setJustification(Justification(Justification::centred));
    };

    m_timeLabel.attachToComponent(&m_timeDisplay, false);
    m_timeLabel.setJustificationType(Justification::centred);
    m_timeLabel.setColour (Label::backgroundColourId, Colours::black);
    m_timeLabel.setColour (Label::textColourId, Colours::orange);
    m_timeLabel.setColour (Label::outlineColourId, Colours::orange);

    m_bpmDisplay.attachToComponent(&m_timeDisplay, true);
    m_bpmDisplay.setJustificationType(Justification::centred);
    m_bpmDisplay.setColour (Label::backgroundColourId, Colours::black);
    m_bpmDisplay.setColour (Label::textColourId, Colours::orange);
    m_bpmDisplay.setColour (Label::outlineColourId, Colours::orange);

    m_bpmLabel.attachToComponent(&m_bpmDisplay, false);
    m_bpmLabel.setJustificationType(Justification::centred);
    m_bpmLabel.setColour (Label::backgroundColourId, Colours::black);
    m_bpmLabel.setColour (Label::textColourId, Colours::orange);
    m_bpmLabel.setColour (Label::outlineColourId, Colours::orange);

    m_playBtn.setImages(false, true, true, m_icons.playBtnNormal, 1, {}, {}, .8, {}, m_icons.playBtnDown, 1, {});
    m_stopBtn.setImages(false, true, true, m_icons.stopBtnNormal, 1, {}, {}, .8, {}, m_icons.stopBtnDown, 1, {});

    m_playBtn.onClick = [this] {playBtnClicked();};
    m_stopBtn.onClick = [this] {stopBtnClicked();};
    m_playBtn.setColour(TextButton::buttonColourId, Colours::blue);
    m_stopBtn.setColour(TextButton::buttonColourId, Colours::red);

    m_playBtn.setEnabled(false);
    m_stopBtn.setEnabled(false);

    m_playBtn.addShortcut(KeyPress(KeyPress::spaceKey));
}

TransportComponent::~TransportComponent()
{
}

void TransportComponent::paint (Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}

void TransportComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    int pad = 8;

    m_playBtn.setBounds(pad, pad, 80, getHeight() - pad * 2);
    m_stopBtn.setBounds(80 + pad, pad, 80, getHeight() - pad * 2);
    int timeDisplayHeight = 28;
    int timeDisplayWidth = 140;
    m_timeDisplay.setBounds((getWidth() - timeDisplayWidth)/ 2, getHeight() - timeDisplayHeight - pad/2, timeDisplayWidth, timeDisplayHeight);
}

void TransportComponent::playBtnClicked()
{
    auto state = m_pPlayer->getPlayState();

    switch (state) {
        case PlayerComponent::PlayState::Playing:
            m_pPlayer->pause();
            m_playBtn.setImages(false, true, true, m_icons.playBtnNormal, 1, {}, {}, .8, {}, m_icons.playBtnDown, 1, {});
            break;

        case PlayerComponent::PlayState::Paused:
        case PlayerComponent::PlayState::Stopped:
            m_pPlayer->play();
            m_playBtn.setImages(false, true, true, m_icons.pauseBtnNormal, 1, {}, {}, .8, {}, m_icons.pauseBtnDown, 1, {});
            break;
    }
}

void TransportComponent::stopBtnClicked()
{
//    auto state = m_pPlayer->getPlayState();
    m_pPlayer->stop();

//    switch (state) {
//        case PlayerComponent::PlayState::Playing:
//            m_pPlayer->pause();
//            break;
//
//        case PlayerComponent::PlayState::Paused:
//        case PlayerComponent::PlayState::Stopped:
//
//            break;
//    }
    m_playBtn.setImages(false, true, true, m_icons.playBtnNormal, 1, {}, {}, .8, {}, m_icons.playBtnDown, 1, {});
    m_playBtn.setEnabled(true);
}

void TransportComponent::init(std::shared_ptr<PlayerComponent> playerComponent) {
    m_pPlayer = std::move(playerComponent);
    m_pPlayer->addActionListener(this);
    m_pPlayer->updateTransportDisplay = [this] {
        updateDisplay();
    };
}

void TransportComponent::actionListenerCallback (const String& message) {
    using namespace Globals::ActionMessage;
    if (message == Stop) {
        stopBtnClicked(); // pause
        stopBtnClicked(); // stop
        m_playBtn.setEnabled(true);
    } else if (message == PlayForExport) {
        stopBtnClicked(); // pause
        stopBtnClicked(); // stop
        playBtnClicked();
        m_playBtn.setEnabled(false);
    } else if (message == EnableTransport) {
        m_playBtn.setEnabled(true);
        m_stopBtn.setEnabled(true);
    }
}

void TransportComponent::convertToSMPTE(SMPTE& smpte, long iPositionInSamples) {
    auto fs     = m_pPlayer->getSampleRate();
    if (fs <= 0)
        return;
    double time = iPositionInSamples / fs;
    time = std::fmax(time, 0);
    smpte.hh    = (int)(std::floor(time) / 3600) % 60;
    smpte.mm    = (int)(std::floor(time) / 60) % 60;
    smpte.ss    = (int)std::floor(time) % 60;
    smpte.ff    = (int)std::floor(time * Globals::GUI::fFramesPerSecond) % (int)Globals::GUI::fFramesPerSecond;
}

void TransportComponent::updateDisplay() {
    SMPTE smpte;
    convertToSMPTE(smpte, m_pPlayer->getCurrentPosition());
    updateTimeDisplay(smpte);

    auto tempo = m_pPlayer->getTempo();
    updateTempoDisplay(tempo);
}

void TransportComponent::updateTimeDisplay(const SMPTE& smpte) {
    char text[12];
    sprintf(text, "%02d:%02d:%02d:%02d", smpte.hh, smpte.mm, smpte.ss, smpte.ff);
    const MessageManagerLock mmLock;
    m_timeDisplay.setText(text, dontSendNotification);
}

void TransportComponent::updateTempoDisplay(double bpm) {
    char text[5];
    sprintf(text, "%03.01f", std::round(bpm));
//    DBG(text);
    m_bpmDisplay.setText(text, dontSendNotification);
}