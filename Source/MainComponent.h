/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Components/TransportBarComponent.h"
#include "Components/Menu.h"
#include "Components/PlayerComponent.h"
#include "Components/TrackViewComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

    bool fileCallback(CommandID);
    void handleFileOpen();

private:
    //==============================================================================
    // Your private member variables go here...
    TransportBarComponent transportBar;
    Menu menu;
    MidiFile midiFile;

    PlayerComponent* player;

    // TrackViewComponents
    TrackViewComponent trackView;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
