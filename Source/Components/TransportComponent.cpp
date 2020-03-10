/*
  ==============================================================================

    TransportBarComponent.cpp
    Created: 11 Jan 2020 11:43:39am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "TransportBarComponent.h"

//==============================================================================
TransportBarComponent::TransportBarComponent(): playBtn("play"), stopBtn("stop")
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    playBtn.onClick = [this] {playBtnClicked();};
    stopBtn.onClick = [this] {stopBtnClicked();};
    playBtn.setColour(TextButton::buttonColourId, Colours::blue);
    stopBtn.setColour(TextButton::buttonColourId, Colours::red);
    playBtn.setEnabled(true);
    stopBtn.setEnabled(false);
    addAndMakeVisible(&playBtn);
    addAndMakeVisible(&stopBtn);
}

TransportBarComponent::~TransportBarComponent()
{
}

void TransportBarComponent::paint (Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}

void TransportBarComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    int pad = 8;
    playBtn.setBounds(pad, pad, 50, getHeight() - pad*2);
    stopBtn.setBounds(getHeight() + pad, pad, 50, getHeight() - pad*2);
}

void TransportBarComponent::playBtnClicked()
{
    playBtn.setButtonText("pause");
    stopBtn.setEnabled(true);
    
}

void TransportBarComponent::stopBtnClicked()
{
    stopBtn.setEnabled(false);
    playBtn.setButtonText("play");
}
