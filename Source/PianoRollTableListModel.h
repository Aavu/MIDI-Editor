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
        
        g.setColour (Colours::grey);
        
        g.setFont (height * 0.7f);
        
        g.drawText (String(rowNumber), 2, 0, width - 4, height, Justification::centredLeft, true);
        
        g.drawRect(g.getClipBounds().toFloat(), 1.5);
        
        g.setColour (Colours::black);
        
    }
    
    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected,
                                        Component* existingComponentToUpdate) override
    {
        // The other columns are editable text columns, for which we use the custom Label component
        auto* pianoRollBox = static_cast<PianoRollComponent*> (existingComponentToUpdate);
        
        // same as above...
        if (pianoRollBox == nullptr)
            pianoRollBox = new PianoRollComponent (*this, rowNumber, columnId);
        
        pianoRollBox->setRowAndColumn (rowNumber, columnId, isRowSelected);
        return pianoRollBox;
    }
    
    //    var getDragSourceDescription (const SparseSet<int>& selectedRows) override
    //    {
    //        // for our drag description, we'll just make a comma-separated list of the selected row
    //        // numbers - this will be picked up by the drag target and displayed in its box.
    //        StringArray rows;
    //
    //        for (int i = 0; i < selectedRows.size(); ++i)
    //            rows.add (String (selectedRows[i] + 1));
    //
    //        return rows.joinIntoString (", ");
    //    }
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
            // single click on the label should simply select the row
            //owner.table.selectRowsBasedOnModifierKeys (row, event.mods, false);
            std::cout << row << ' ' << columnId << std::endl;
            if (owner.getNote(row, columnId))
            {
                //owner.deleteNote(row, columnId); // do not delete
            }
            else
            {
                PianoRollNote* newNote = new PianoRollNote(0,0);
                owner.addNote(row, columnId, newNote);
                owner.addSelectedNotes(newNote);
            }
            repaint();
        }
        
        // Our demo code will call this when we may need to update our contents
        void setRowAndColumn (const int newRow, const int newColumn, bool isRowSelected)
        {
            row = newRow;
            columnId = newColumn;
            //std::cout << row << ' ' << columnId << ' ' << isRowSelected << std::endl;
        }
        
        void paint (Graphics& g) override
        {
            g.setColour (Colours::grey);
            
            //g.setFont (height * 0.7f);
            
            // g.drawText (String(rowNumber), 2, 0, width - 4, height, Justification::centredLeft, true);
            
            g.drawRect(g.getClipBounds().toFloat(), 0.5);
            
            g.setColour (Colours::green);
            if (owner.getNote(row, columnId)) {
                //std::cout << "draw " << row << ' ' << columnId << std::endl;
                g.fillRoundedRectangle(g.getClipBounds().toFloat(), 3);
            }
            
            g.setColour (Colours::black);
        }
        
    private:
        PianoRollTableListBoxModel& owner;
        int row, columnId;
    };
};
