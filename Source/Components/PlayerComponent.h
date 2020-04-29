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
class PlayerComponent : public Component, public ActionBroadcaster, public ActionListener, public Timer
{
public:
    enum class PlayState {
        Playing,
        Paused,
        Stopped
    };
    //==============================================================================

    PlayerComponent();
    ~PlayerComponent() override;
    //------------------------------------------------------------------------------
    void paint (Graphics&) override;
    void resized() override;
    //------------------------------------------------------------------------------
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill);
    //------------------------------------------------------------------------------
    MidiMessageSequence * getMidiMessageSequence() const;
    void setMidiMessageSequence(MidiMessageSequence* midiMsgSeq);
    PlayState getPlayState() {return m_playState;}
    double getSampleRate() {return m_fSampleRate;}
    long getMaxBufferLength() {return m_iMaxBufferLength;}
    long getCurrentPosition() {return m_iCurrentPosition;}
    void setCurrentPosition(long value);
    double getCurrentPositionInQuarterNotes();
    void setCurrentPositionByQuarterNotes(double newPositionInQuarterNotes);
    void resetCurrentPosition();
    //------------------------------------------------------------------------------
    void setTimeFormat(int timeFormat);
    MidiMessageSequence& getTempoEvents();
    MidiMessageSequence& getTempoEventsInSecs();
    double convertQuarterNoteToSec(double positionInQuarterNotes);
    double convertSecToQuarterNote(double positionInSec);
    //------------------------------------------------------------------------------
    void play();
    void pause();
    void stop();
    void allNotesOff();
    //------------------------------------------------------Midi-note-operations----
    // void addNote(); // TODO: Define
    void deleteNote(int iNoteOnEventIndex);
    /*
     * Changes noteOn and noteOff timestamps.
     * Duration is set to fNoteDurationInQuarterNote if provided, else it is kept the same.
     */
    void updateNote(int iNoteOnEventIndex, double fNewNoteOnTimestampInQuarterNotes, double fNoteDurationInQuarterNotes = -1, int iNewNoteNumber = -1);
    //==============================================================================

    std::function<void()> updateTimeDisplay = nullptr;

private:
    static String getAbsolutePathOfProject(const String& projectFolderName = "MIDI-Editor");
    //------------------------------------------------------------------------------
    void timerCallback() override;
    void actionListenerCallback (const String& message) override;
    //------------------------------------------------------------------------------
    void initSynth();
    void fillMidiBuffer(int iNumSamples);
    //==============================================================================

    MidiMessageSequence m_TempoEvents;          // timestamp in ticks
    MidiMessageSequence m_TempoEventsInSec;     // timestamp in seconds
    double m_fTempo = 120;
    int m_iTimeFormat;
    double m_fSampleRate = 0;

    MidiMessageSequence* m_midiMessageSequence = nullptr;
    int m_iMidiEventReadIdx = 0;
    int m_iMaxMidiEvents = 0;

    MidiBuffer m_midiBuffer;
    MidiBuffer m_currentMidiBuffer;
    int m_iSamplesPerBlockExpected = 0;
    long m_iCurrentPosition = 0;
    long m_iLastRetrievedPosition = 0;
    long m_iMaxBufferLength = 0;
    const double m_fMinBufferLengthInSec = 10.0;

    PlayState m_playState = PlayState::Stopped;
    SoundFontGeneralMidiSynth m_synth;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerComponent)
};
