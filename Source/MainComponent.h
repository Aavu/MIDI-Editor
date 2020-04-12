/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Components/TransportComponent.h"
#include "Components/MenuComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/TrackViewComponent.h"

#include "Synth/MidiSynth.h"
#include "Synth/SfzMidiSynth.h"

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
    ~MainComponent() override;

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
    TransportComponent m_transportBar;
    MenuComponent m_menu;
    MidiFile m_midiFile;

    PlayerComponent* m_pPlayer;

    // TrackViewComponents
    TrackViewComponent m_trackView;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
