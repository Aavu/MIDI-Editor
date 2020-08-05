/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Components/Globals.h"
#include "Components/TransportComponent.h"
#include "Components/MenuComponent.h"
#include "Components/PlayerComponent.h"
#include "Components/TrackViewComponent.h"
#include "Components/AudioExportComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent   : public AudioAppComponent, public ActionBroadcaster, public ActionListener
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

private:
    void handleFileOpen();
    void handleExportAudio();
    void handleExportMidi();

    void actionListenerCallback (const String& message) override;

    //==============================================================================
    // Your private member variables go here...
    double m_fSampleRate = 0;
    int m_iBitDepth = 0;
    int m_iNumChannels = 2;
    int m_iNextSampleNum = 0;

    std::atomic<bool> m_bExporting{false};

    MidiMessageSequence* sequenceCopy =  nullptr;
    
    TransportComponent m_transportBar;
    MenuComponent m_menu;
    MidiFile m_midiFile;

    std::shared_ptr<PlayerComponent> m_pPlayer;

    // TrackViewComponents
    std::unique_ptr<TrackViewComponent> m_pTrackView;

    // Audio Export
    std::unique_ptr<AudioExportComponent> m_pAudioExporter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
