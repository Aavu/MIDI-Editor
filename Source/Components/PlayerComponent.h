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
#include "GUIComponent/ScrollablePianoRollComponent/PianoRollNote.h"
#include "Util.h"

class PianoRollNote;
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

    static std::shared_ptr<PlayerComponent> getInstance();
    //------------------------------------------------------------------------------
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
    PlayState getPlayState() const;
    double getSampleRate() const;
    long getMaxBufferLength() const;
    long getCurrentPosition() const;
    void setCurrentPosition(long value);
    double getCurrentPositionInQuarterNotes();
    void setCurrentPositionByQuarterNotes(double newPositionInQuarterNotes);
    void resetCurrentPosition();
    void setTimeFormat(int timeFormat);
    double getTempo();
    MidiMessageSequence& getTempoEvents();
    MidiMessageSequence& getTempoEventsInSecs();
    void clearTempoEvents();
    //------------------------------------------------------------------------------
    bool isSequenceLoaded();
    double convertQuarterNoteToSec(double positionInQuarterNotes);
    double convertSecToQuarterNote(double positionInSec);
    //------------------------------------------------------------------------------
    void play();
    void pause();
    void stop();
    void allNotesOff();
    //------------------------------------------------------------------------------
    void addNote(PianoRollNote * pPianoRollNote); // TODO: Define
    void deleteNote(int iNoteOnEventIndex);
    /*
     * Changes noteOn and noteOff timestamps.
     * Duration is set to fNoteDurationInQuarterNote if provided, else it is kept the same.
     */
    void updateNote(MidiMessageSequence::MidiEventHolder *pNoteOnEvent, MidiMessageSequence::MidiEventHolder *pNoteOffEvent, double fNewNoteOnTimestampInQuarterNotes, double fNoteDurationInQuarterNotes = -1, int iNewNoteNumber = -1);
    //==============================================================================

    std::function<void()> updateTransportDisplay = nullptr;

private:
    static std::shared_ptr<PlayerComponent> m_pInstance;

    static String getAbsolutePathOfProject(const String& projectFolderName = "MIDI-Editor"); // TODO: Remove later. Use function from CUtil instead.
    //------------------------------------------------------------------------------
    void initSynth();

    void fillMidiBuffer(int iNumSamples);

    void addMessageToTempoBuffer(const MidiMessage& message);
    void addAllTempoMessagesToBuffer();
    void updateTempo();
    /*
     * Resets all m_midiBuffer and m_midiMessageSequence indices and clears the buffer.
     */
    void resetPlayer();

    void timerCallback() override;
    void actionListenerCallback (const String& message) override;

    //==============================================================================

    MidiMessageSequence m_TempoEvents;          // timestamp in ticks
    MidiMessageSequence m_TempoEventsInSec;     // timestamp in seconds
    std::unique_ptr<MidiBuffer::Iterator> m_pTempoIterator;
    double m_fTempo = 120;
    int m_iTimeFormat;
    double m_fSampleRate = 0;

    double m_fCurrentTempo = 120;
    MidiBuffer m_tempoEventBuffer;

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
    bool m_bSequenceLoaded = false;

    SoundFontGeneralMidiSynth m_synth;

    CriticalSection m_criticalSection;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlayerComponent)
};
