/*
  ==============================================================================

    PianoRollNote.h
    Created: 26 Feb 2020 2:23:00pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Globals.h"

// midi message associated with that note
// TODO: add members and functions later
class NoteMessage {
  // empty now
};
class SelectedNoteList;

// TODO: fix dragging behavior
class PianoRollNote: public TextButton, public ComponentBoundsConstrainer
{
public:
    PianoRollNote(int row_n, float offset_n, float length_n = 1, int velocity_n = Globals::midiNoteNum, NoteMessage *noteMessage_n = 0);
    
    ~PianoRollNote();
    
    void resized() override;
    
    void mouseDown (const MouseEvent& event) override;
    
    void mouseDrag (const MouseEvent& event) override;
    
    void mouseEnter(const MouseEvent& event) override;
    
    int getRow();

    float getOffset();
    float getLength();
    
    NoteMessage* getNoteMessage();
    
    bool ifInit();
    
    void paintButton (Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    std::function<void(PianoRollNote*, int)> changePitch;
    
private:
    
    bool                init = false;
    int                 row;                          // midi number (0~127)
    float               offset;
    float               length;                      // relative length (1 means 1 quarter note)
    int                 velocity;
    int                 boxWidth;
    int                 boxHeight;
    
    NoteMessage         *noteMessage;
    
    // dragger
    ComponentDragger                myDragger;
    ResizableBorderComponent*       border;
    ComponentBoundsConstrainer*     constrainer;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRollNote)
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
    Array<PianoRollNote*>               *noteList;
    SelectedNoteList                    *selected;
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
    Array<PianoRollNote*>               *selected;
    NoteList                            *noteList;
};
