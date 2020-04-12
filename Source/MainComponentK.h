/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "Synth/MidiSynth.h"
#include "Synth/SfzMidiSynth.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponentK   : public AudioAppComponent,
                         private Timer
{
public:
    //==============================================================================
    MainComponentK();
    ~MainComponentK() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    static String getAbsolutePathOfProject(const String& projectFolderName = "MIDI-Editor");

    SfzSynthAudioSource m_synthAudioSource;
    MidiKeyboardState m_keyboardState;
    MidiKeyboardComponent m_keyboardComponent;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponentK)
};
