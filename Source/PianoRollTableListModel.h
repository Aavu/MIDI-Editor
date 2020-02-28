/*
  ==============================================================================

    PianoRollTableListModel.h
    Created: 26 Feb 2020 2:34:28pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

class NoteList;
class SelectedNoteList;

class PianoRollTableListBox: public TableListBox
{
public:
    PianoRollTableListBox(const String& componentName = String(),
                          TableListBoxModel* model = nullptr):
        TableListBox(componentName, model)
    {}
    
    void setNoteList(NoteList *noteList_n)
    {
        noteList = noteList_n;
    }
    
    void setSelectedNoteList(SelectedNoteList *selected_n)
    {
        selected = selected_n;
    }

    bool keyPressed(const KeyPress &     key) override
    {
        if (key.getKeyCode() == 127) // delete key
        {
            selected->removeSelectedNotes();
            repaint();
            return true;
        }
        else {
            return false;
        }
    }
private:
    NoteList                    *noteList;
    SelectedNoteList            *selected;
};

class PianoRollTableListBoxModel: public TableListBoxModel
{
public:
    PianoRollTableListBoxModel()
    {
    }
    
    ~PianoRollTableListBoxModel()
    {
    }
    
    void setNoteList(NoteList *noteList_n)
    {
        noteList = noteList_n;
    }
    
    void setSelectedNoteList(SelectedNoteList *selected_n)
    {
        selected = selected_n;
    }
    
    // The following methods implement the necessary virtual functions from ListBoxModel,
    // telling the listbox how many rows there are, painting them, etc.
    int getNumRows() override
    {
        return 128;
    }
    
    // This is overloaded from TableListBoxModel, and should fill in the background of the whole row
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        g.fillAll (Colours::lightblue);
    }
    
    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override
    {
        // control does not reach here because all the components are customized
    }
    
    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected,
                                        Component* existingComponentToUpdate) override
    {
        auto* pianoRollBox = static_cast<PianoRollComponent*> (existingComponentToUpdate);
        
        if (pianoRollBox == nullptr)
            pianoRollBox = new PianoRollComponent (*this, rowNumber, columnId);
        
        pianoRollBox->setRowAndColumn (rowNumber, columnId, isRowSelected);
        return pianoRollBox;
    }
    
private:
    NoteList                            *noteList;
    SelectedNoteList                    *selected;
    
    class PianoRollComponent  : public Component
    {
    public:
        PianoRollComponent (PianoRollTableListBoxModel& td, int row_n, int col_n)  : owner (td)
        {
            row = row_n;
            columnId = col_n;
        }
        
        void mouseDown (const MouseEvent& event) override
        {
            if (owner.noteList->getNote(row, columnId))
            {
                PianoRollNote* curNote = owner.noteList->getNote(row, columnId);
                owner.selected->selectOneNote(curNote);
            }
            else
            {
                PianoRollNote* newNote = new PianoRollNote(row,columnId);
                owner.noteList->addNote(row, columnId, newNote);
                owner.selected->selectOneNote(newNote);
            }
            repaint();
        }
        
        void setRowAndColumn (const int newRow, const int newColumn, bool isRowSelected)
        {
            row = newRow;
            columnId = newColumn;
        }
        
        void paint (Graphics& g) override
        {
            g.setColour (Colours::grey);
            
            g.drawRect(g.getClipBounds().toFloat(), 0.5);
            
            g.setColour (Colours::green);
            if (owner.noteList->getNote(row, columnId)) {
                g.fillRoundedRectangle(g.getClipBounds().toFloat(), 3);
            }
            
            g.setColour (Colours::black);
        }
        
    private:
        PianoRollTableListBoxModel& owner;
        int row, columnId;
    };
};
