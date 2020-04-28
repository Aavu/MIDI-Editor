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

//==============================================================================
/*
*/
class PlayerComponent : public Component, public ActionBroadcaster
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
    
    MidiMessageSequence& getTempoEvents();
    
    MidiMessageSequence& getTempoEventsInSecs();
    
    double getCurrentPositionInQuarterNotes();
    
    void setCurrentPositionByQuarterNotes(double newPositionInQuarterNotes);
    
    void setTimeFormat(int timeFormat);

    long getMaxBufferLength() {
        return m_iMaxBufferLength;
    }

    void setCurrentPosition(long value);
    void resetCurrentPosition();

    void updateNoteTimestamp(int iEventIndex, double fNewTimestamp);

private:
    static String getAbsolutePathOfProject(const String& projectFolderName = "MIDI-Editor");

    void initSynth();

    //void addMessageToBuffer(const MidiMessage& message);
    //void addAllSequenceMessagesToBuffer();
    void fillMidiBuffer(int iNumSamples);

    long m_iMaxBufferLength = 0;

    double m_fTempo = 120;
    int m_iTimeFormat;
    MidiMessageSequence m_TempoEvents;          // timestamp in ticks
    MidiMessageSequence m_TempoEventsInSec;     // timestamp in seconds

    MidiMessageSequence* m_midiMessageSequence = nullptr;
    MidiMessageSequence::MidiEventHolder * const * m_midiEventHolder = nullptr;
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

    SfzLoader m_sfzLoader;
    SfzLoader m_sfzLoader1;
    SfzSynth m_synth;

    constexpr static int kiNumVoices = 24;
    constexpr static int kiNumChannels = 16;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerComponent)
};
