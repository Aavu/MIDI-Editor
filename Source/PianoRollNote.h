/*
  ==============================================================================

    PianoRollNote.h
    Created: 26 Feb 2020 2:23:00pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

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
    PianoRollNote(int row_n, float offset_n, float length_n = 1, int velocity_n = Globals::midiNoteNum, NoteMessage *noteMessage_n = 0):
        row(row_n),
        offset(offset_n), length(length_n),
        velocity(velocity_n), noteMessage(noteMessage_n),
        border(0)
    {
        init = true;
        boxWidth = Globals::noteWidth;
        boxHeight = Globals::noteHeight;
        
        border = new ResizableBorderComponent(this, NULL);
        addChildComponent(border);
        border->setBorderThickness (BorderSize<int> (0,0,0,1));
        border->setBounds(this->getBounds());
        border->setAlpha(0.1);
        border->setVisible(true);
        
        constrainer = new ComponentBoundsConstrainer();
        constrainer->setMaximumHeight(boxHeight);
        std::cout << "create " << row << ' ' << offset_n << std::endl;
    }
    
    ~PianoRollNote()
    {
        if (noteMessage)
            delete noteMessage;
        if (border)
            delete border;
        if (constrainer)
            delete constrainer;
        std::cout << "delete " << row << ' ' << offset << std::endl;
    }
    
    void resized() override
    {
        // TODO: not working now
        if (border)
        {
            border->setBounds(0,0,getWidth(),getHeight());
            length = 1.F*getWidth() / boxWidth;
            std::cout << length << std::endl;
            repaint();
        }
        if (constrainer)
        {
            auto newBounds = getBoundsInParent();
            setBounds (newBounds);
            constrainer->setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
        }
    }
    
    void mouseDown (const MouseEvent& event) override
    {
        // add to selectedNoteList
        getParentComponent()->mouseDown(event);
        myDragger.startDraggingComponent (this, event);
    }
    
    void mouseDrag (const MouseEvent& event) override
    {
        myDragger.dragComponent (this, event, constrainer);
    }
    
    void mouseEnter(const MouseEvent& event) override
    {
        std::cout << "mouse enter: " << row << std::endl;
    }
    
    int getRow() { return row; }

    float getOffset() { return offset; }
    float getLength() { return length; }
    
    NoteMessage* getNoteMessage() { return noteMessage; }
    
    bool ifInit() { return init; }
    
    void paintButton (Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        g.setColour (Colours::limegreen);
        g.fillRoundedRectangle(0, 0, length*boxWidth, boxHeight, 4);
        g.setColour (Colours::lime);
        g.drawRoundedRectangle(0, 0, length*boxWidth, boxHeight, 4, 1);
    }
    
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
};

class NoteList
{
public:
    NoteList(SelectedNoteList *selected_n)
    {
        selected = selected_n;
        noteList = new Array<PianoRollNote*> [Globals::midiNoteNum];
        for (int i = 0; i < Globals::midiNoteNum; i++) {
            noteList[i] = Array<PianoRollNote*>();
        }
    }
    ~NoteList()
    {
        for (int i = 0; i < Globals::midiNoteNum; i++) {
            for (int j = 0; j < noteList[i].size(); j++)
                if (getNote(i, j))
                    deleteNote(i, j);
            noteList[i].clear();
        }
        delete [] noteList;
    }
    
    PianoRollNote* getNote(int row, int idx)
    {
        return noteList[row][idx];
    }
    
    Array<PianoRollNote*> getNotesByRow(int row)
    {
        return noteList[row];
    }
    
    void addNote(int row, PianoRollNote* pianoRollNote)
    {
        std::cout << "add Note to row: " << row << " offset: " << pianoRollNote->getOffset() << std::endl;

        noteList[row].add(pianoRollNote);
    }
    
    void deleteNote(int row, int idx)
    {
        PianoRollNote* noteToBeRemoved = getNote(row, idx);
        if (noteToBeRemoved)
        {
            noteList[row].remove(idx);
            delete noteToBeRemoved;
        }
    }
    
    void deleteNote(PianoRollNote *noteToBeRemoved)
    {
        int row = noteToBeRemoved->getRow();
        noteList[row].removeFirstMatchingValue(noteToBeRemoved);
        delete noteToBeRemoved;
    }
    
private:
    Array<PianoRollNote*>               *noteList;
    SelectedNoteList                    *selected;
};

class SelectedNoteList
{
public:
    SelectedNoteList(NoteList* noteList_n)
    {
        selected = new Array<PianoRollNote*> ();
        noteList = noteList_n;
    }
    ~SelectedNoteList()
    {
        selected->clear();
        delete selected;
    }
    void selectOneNote(PianoRollNote* newNote)
    {
        selected->clear();
        selected->add(newNote);
    }
    
    void addSelectedNotes(PianoRollNote* newNote)
    {
        selected->add(newNote);
    }
    
    void removeSelectedNotes()
    {
        PianoRollNote* noteToBeRemoved = 0;
        while (!selected->isEmpty())
        {
            noteToBeRemoved = selected->removeAndReturn(0);
            noteList->deleteNote(noteToBeRemoved);
        }
    }
private:
    Array<PianoRollNote*>               *selected;
    NoteList                            *noteList;
};
