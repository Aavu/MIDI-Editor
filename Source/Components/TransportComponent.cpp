/*
  ==============================================================================

    TransportComponent.cpp
    Created: 11 Jan 2020 11:43:39am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TransportComponent.h"

//==============================================================================
TransportComponent::TransportComponent():
        m_playBtn("play"),
        m_stopBtn("stop"),
        m_pPlayer(nullptr)
{
    m_playBtn.onClick = [this] {playBtnClicked();};
    m_stopBtn.onClick = [this] {stopBtnClicked();};
    m_playBtn.setColour(TextButton::buttonColourId, Colours::blue);
    m_stopBtn.setColour(TextButton::buttonColourId, Colours::red);

    addAndMakeVisible(&m_playBtn);
    addAndMakeVisible(&m_stopBtn);
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
    m_playBtn.setBounds(pad, pad, 50, getHeight() - pad * 2);
    m_stopBtn.setBounds(getHeight() + pad, pad, 50, getHeight() - pad * 2);
}

void TransportComponent::playBtnClicked()
{
    auto state = m_pPlayer->getPlayState();

    switch (state) {
        case PlayerComponent::PlayState::Playing:
            m_pPlayer->pause();
            m_playBtn.setButtonText("Resume");
            break;

        case PlayerComponent::PlayState::Paused:
        case PlayerComponent::PlayState::Stopped:
            m_pPlayer->play();
            m_playBtn.setButtonText("Pause");
            break;
    }
}

void TransportComponent::stopBtnClicked()
{
    auto state = m_pPlayer->getPlayState();

    switch (state) {
        case PlayerComponent::PlayState::Playing:
            m_pPlayer->pause();
            m_playBtn.setButtonText("Resume");
            break;

        case PlayerComponent::PlayState::Paused:
        case PlayerComponent::PlayState::Stopped:
            m_pPlayer->stop();
            m_playBtn.setButtonText("Play");
            break;
    }
}

void TransportComponent::init(PlayerComponent* playerComponent) {
    m_pPlayer = playerComponent;
    m_pPlayer->addActionListener(this);
}

void TransportComponent::actionListenerCallback (const String& message) {
    if (message == "stop") {
        stopBtnClicked(); // pause
        stopBtnClicked(); // stop
    } else if (message == "playForExport") {
        stopBtnClicked(); // pause
        stopBtnClicked(); // stop
        playBtnClicked();
    }
    DBG(message);
}

bool TransportComponent::getCurrentPosition (CurrentPositionInfo& result) {
    if (!m_pPlayer)
        return false;

    auto pos = m_pPlayer->getCurrentPosition(AudioPlayHead::CurrentPositionInfo());
    result.resetToDefault();
    result.isPlaying = (m_pPlayer->getPlayState() == PlayerComponent::PlayState::Playing);
    result.timeInSamples = pos;
    result.bpm = bpm;
    result.timeInSeconds = pos/m_pPlayer->getSampleRate();
    result.timeSigNumerator = 4;
    result.timeSigDenominator = 4;
    result.editOriginTime = 0;
    result.isLooping = false;
    result.isRecording  = false;
    return true;
}