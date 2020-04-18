/*
  ==============================================================================

    PlayerComponent.h
    Created: 12 Jan 2020 8:47:56pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <memory>
#include <vector>

#include "../Synth/MidiSynth.h"
#include "../Synth/SfzMidiSynth.h"

#include "../Synth/SynthSound.h"
#include "../Synth/SynthVoice.h"


//==============================================================================
/*
*/

//TODO: Implement play pause stop states.
//TODO: Use separate thread for timing instead of Timer class.?

class PlayerComponent    : public Component
{
public:
    PlayerComponent();
    ~PlayerComponent() override;

    void paint (Graphics&) override;
    void resized() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);

    void setMidiMessageSequence(const MidiMessageSequence* midiMsgSeq);
    void play();
    void pause();
    void stop();

    enum class PlayState {
        Playing,
        Paused,
        Stopped
    };

    PlayState getPlayState();
    void updateNumSamples(const AudioSourceChannelInfo &bufferToFill);
    void resetCurrentPosition();

protected:
    CriticalSection lock;

private:
    static String getAbsolutePathOfProject(const String& projectFolderName = "MIDI-Editor");

    void initSynth();

    void addMessageToBuffer(const MidiMessage& message);
    void addAllSequenceMessagesToBuffer();

    //==============================================================================
    const MidiMessageSequence* m_midiMessageSequence = nullptr;
    MidiBuffer m_midiBuffer;
    MidiBuffer m_currentMidiBuffer;
    std::unique_ptr<MidiBuffer::Iterator> m_pIterator;
    double m_fSampleRate = 0;
    int m_iSamplesPerBlockExpected = 0;
    PlayState m_playState = PlayState::Stopped;
    int m_iCurrentPosition = 0;

    // Synth
//    SfzSynthAudioSource m_synthAudioSource;
//    MidiKeyboardState m_keyBoardState;

    Synthesiser m_synth;
    constexpr static int kiNumVoices = 5;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerComponent)
};
