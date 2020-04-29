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

#include "../Synth/SfzMidiSynth.h"
#include "Globals.h"
#include "Util.h"

//==============================================================================
/*
*/
class PlayerComponent : public Component, public ActionBroadcaster, public ActionListener, public Timer
{
public:
    PlayerComponent();
    ~PlayerComponent() override;

    void paint (Graphics&) override;
    void resized() override;

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);

    void setMidiMessageSequence(MidiMessageSequence* midiMsgSeq);

    void play();
    void pause();
    void stop();

    void allNotesOff();

    enum class PlayState {
        Playing,
        Paused,
        Stopped
    };

    PlayState getPlayState() {
        return m_playState;
    }

    long getCurrentPosition() {
        return m_iCurrentPosition;
    }

    double getSampleRate() {
        return m_fSampleRate;
    }

    double getTempo() {
        return m_fCurrentTempo;
    }

    long getMaxBufferLength() {
        return m_iMaxBufferLength;
    }

    void setCurrentPosition(long value);

    void resetCurrentPosition();

    std::function<void()> updateTransportDisplay = nullptr;


private:
    void initSynth();

    void fillMidiBuffer(int iNumSamples);
    void addMessageToTempoBuffer(const MidiMessage& message);
    void addAllTempoMessagesToBuffer();

    void updateTempo();


    void timerCallback() override;

    void actionListenerCallback (const String& message) override;

    long m_iMaxBufferLength = 0;

    double m_fCurrentTempo = 120;
    MidiBuffer m_tempoEventBuffer;

    MidiMessageSequence* m_midiMessageSequence = nullptr;

    int m_iMidiEventReadIdx = 0;
    int m_iMaxMidiEvents = 0;

    long m_iLastRetrievedPosition = 0;



    MidiBuffer m_midiBuffer;
    MidiBuffer m_currentMidiBuffer;
    std::unique_ptr<MidiBuffer::Iterator> m_pIterator;
    double m_fSampleRate = 0;
    int m_iSamplesPerBlockExpected = 0;
    PlayState m_playState = PlayState::Stopped;
    long m_iCurrentPosition = 0;

    SoundFontGeneralMidiSynth m_synth;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerComponent)
};
