/*
  ==============================================================================

    PianoRollNote.h
    Created: 26 Feb 2020 2:23:00pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Globals.h"
#include "../../PlayerComponent.h"

// midi message associated with that note
// TODO: add members and functions later
class NoteMessage {
    // empty for now
    //  maybe put noteOn and noteOff MidiMessage here
};
class SelectedNoteList;
class NoteList;
class PianoRollBorderComponent;


class PianoRollNote: public TextButton, public ComponentBoundsConstrainer
{
public:
    PianoRollNote(PlayerComponent* player, int row_n, float offset_n, float length_n = 1, int velocity_n = Globals::PianoRoll::midiNoteNum, int orig_idx_on = -1, int orig_idx_off = -1, NoteMessage *noteMessage_n = 0);
    
    ~PianoRollNote();
    
    void resized() override;
    
    void mouseDown (const MouseEvent& event) override;

    void mouseUp (const MouseEvent &event) override ;


    void mouseDrag (const MouseEvent& event) override;
    
    void mouseEnter(const MouseEvent& event) override;
    
    int getRow();

    float getOffset();
    float getLength();
    
    NoteMessage* getNoteMessage();
    
    bool ifInit();
    
    void paintButton (Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    std::function<void(PianoRollNote*, int)> changePitch;
    std::function<void()> hightlightRow;
    
private:
    
    bool                m_bInit = false;
    int                 m_iOrigIdxOn;                    // index of noteOn in the MidiMessageSequence
    int                 m_iOrigIdxOff;                   // index of noteOff in the MidiMessageSequence
    int                 m_iRow;                          // midi number (0~127)
    float               m_fOffset;
    float               m_fLength;                      // relative length (1 means 1 quarter note)
    uint8                 m_iVelocity;
    int                 m_iBoxWidth;
    int                 m_iBoxHeight;
    
    NoteMessage         *m_pNoteMessage;
    
    // player
    PlayerComponent     *m_pPlayer;
    
    // dragger
    ComponentDragger                m_pMyDragger;
    PianoRollBorderComponent*       m_pBorder;
    ComponentBoundsConstrainer*     m_pConstrainer;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRollNote)
};

class PianoRollBorderComponent: public ResizableBorderComponent
{
public:
    PianoRollBorderComponent(Component *componentToResize, ComponentBoundsConstrainer *constrainer):
        ResizableBorderComponent(componentToResize, constrainer)
    {
    }
    
    void mouseUp (const MouseEvent &event) override
    {
        getParentComponent()->mouseUp(event);
    }
};

class NoteList
{
public:
    NoteList(SelectedNoteList *selected_n);

    ~NoteList();
    
    PianoRollNote* getNote(int row, int idx);
    
    Array<PianoRollNote*> getNotesByRow(int row);
    
    void addNote(int row, PianoRollNote* pianoRollNote);
    
    void detachNote(int row, PianoRollNote* noteToMove);
    
    void moveNote(int row, PianoRollNote* noteToMove, int direction);
    
    void deleteNote(int row, int idx);
    
    void deleteNote(PianoRollNote *noteToBeRemoved);
    
private:
    Array<PianoRollNote*>               *m_pNoteList;
    SelectedNoteList                    *m_pSelected;
};

class SelectedNoteList
{
public:
    SelectedNoteList(NoteList* noteList_n);

    ~SelectedNoteList();
    
    void selectOneNote(PianoRollNote* newNote);
    
    void addSelectedNotes(PianoRollNote* newNote);
    
    void removeSelectedNotes();
    
private:
    Array<PianoRollNote*>               *m_pSelected;
    NoteList                            *m_pNoteList;
};
