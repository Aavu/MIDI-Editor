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

// TODO: fix dragging behavior
class PianoRollNote: public TextButton
{
public:
    PianoRollNote(int row_n, int column_n, Rectangle<int> bounds, int length_n = 1, int velocity_n = Globals::midiNoteNum, int offset_n = 0, NoteMessage *noteMessage_n = 0):
        row(row_n), column(column_n),
        offset(offset_n), length(length_n),
        velocity(velocity_n), noteMessage(noteMessage_n),
        border(0)
    {
        //setBounds(bounds);
        setPaintingIsUnclipped(true);
        setSize(40,10);
        border = new ResizableBorderComponent(this, NULL);
    }
    ~PianoRollNote()
    {
        if (noteMessage)
            delete noteMessage;
        if (border)
            delete border;
    }
    
//    void resized() override
//    {
//        // TODO: not working now
//        if (border)
//            border->setBounds(0,0,getWidth(),getHeight());
//        //border->setBorderThickness (BorderSize<int>(3));
//        //border->setVisible(1);
//    }
    
    void updateBounds(Rectangle<int> bd_n)
    {
        std::cout << "updatebounds: " << bd_n.getWidth() << ' ' << bd_n.getHeight() << std::endl;
        setBounds(bd_n);
        repaint();
    }
    
    void mouseDown (const MouseEvent& event) override
    {
        getParentComponent()->mouseDown(event);
    }
    
    int getRow()
    {
        return row;
    }
    int getColumn()
    {
        return column;
    }
    int getOffset()
    {
        return offset;
    }
    int getLength()
    {
        return length;
    }
    NoteMessage* getNoteMessage()
    {
        return noteMessage;
    }
    
    void paintButton (Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        
        g.setColour (Colours::green);
        g.fillRoundedRectangle(g.getClipBounds().toFloat(), 3);
    }
    
//    void mouseDown (const MouseEvent& e)
//    {
//        myDragger.startDraggingComponent (this, e);
//    }
//
//    void mouseDrag (const MouseEvent& e)
//    {
//        myDragger.dragComponent (this, e, nullptr);
//    }
private:
    
    int                 row;                          // midi number (0~127)
    int                 column;                       // idx of column (0~inf)
    int                 offset;
    int                 length;                      // relative length (1 means 1 quarter note)
    int                 velocity;
    
    NoteMessage         *noteMessage;
    
    // dragger
    ComponentDragger myDragger;
    ResizableBorderComponent* border;
};

class NoteList
{
public:
    NoteList()
    {
        noteList = new Array<PianoRollNote*> [Globals::midiNoteNum];
        for (int i = 0; i < Globals::midiNoteNum; i++) {
            noteList[i] = Array<PianoRollNote*>();
            noteList[i].insertMultiple(0, 0, 40);
        }
    }
    ~NoteList()
    {
        for (int i = 0; i < Globals::midiNoteNum; i++)
            for (int j = 0; j < Globals::tickNum; j++)
                if (getNote(i, j))
                    deleteNote(i, j);
        delete [] noteList;
    }
    
    PianoRollNote* getNote(int row, int col)
    {
        //std::cout << "get Note: " << row << ' ' << col << std::endl;
        return noteList[row][col];
    }
    
    void addNote(int row, int col, PianoRollNote* pianoRollNote)
    {
        std::cout << "add Note: " << row << ' ' << col << std::endl;
        deleteNote(row, col);
        noteList[row].set(col, pianoRollNote);
    }
    
    void deleteNote(int row, int col)
    {
        //std::cout << "delete Note: " << row << ' ' << col << std::endl;
        if (getNote(row, col)) {
            delete getNote(row, col);
            noteList[row].set(col, 0);
        }
    }
    
    void deleteNote(PianoRollNote *noteToBeRemoved)
    {
        int row = noteToBeRemoved->getRow();
        int col = noteToBeRemoved->getColumn();
        deleteNote(row, col);
    }
    
private:
    Array<PianoRollNote*>               *noteList;
};

class SelectedNoteList
{
public:
//    SelectedNoteList()
//    {
//        selected = new Array<PianoRollNote*> ();
//    }
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
