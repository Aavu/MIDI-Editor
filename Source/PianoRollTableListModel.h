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

class PianoRollTableListBox: public TableListBoxModel, public TableListBox, public NoteList, public SelectedNoteList
{
public:
    PianoRollTableListBox(const String& componentName = String(),
                          TableListBoxModel* model = nullptr):
        TableListBox(componentName, this),
        SelectedNoteList(this)
    {
//        addAndMakeVisible(sidebar);
//        sidebar.setColour (TextButton::buttonColourId, Colours::limegreen);
//        sidebar.setButtonText ("note");
//        sidebar.setBounds(1.5,1.5,40,10);
//        sidebar.setOpaque (false);
    }

    bool keyPressed(const KeyPress &     key) override
    {
        std::cout << "key code: " << key.getKeyCode() << std::endl;
        if (key.getKeyCode() == 127) // delete key
        {
            removeSelectedNotes();
            repaint();
            return true;
        }
        else {
            return false;
        }
    }
    
    // model
    // The following methods implement the necessary virtual functions from ListBoxModel,
    // telling the listbox how many rows there are, painting them, etc.
    int getNumRows() override
    {
        return Globals::midiNoteNum;
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
        auto* pianoRollBox = static_cast<NoteComponent*> (existingComponentToUpdate);
        
        if (pianoRollBox == nullptr)
            pianoRollBox = new NoteComponent (*this, rowNumber, columnId);

        pianoRollBox->setRowAndColumn (rowNumber, columnId, isRowSelected);
        
        return pianoRollBox;
    }
    
    
private:
    TextButton                  sidebar;
    
    //model
    class NoteComponent  : public Component
    {
    public:
        NoteComponent (PianoRollTableListBox& td, int row_n, int col_n)  : owner (td)
        {
            row = row_n;
            columnId = col_n;
            
            if (owner.getNote(row, columnId))
            {
                PianoRollNote* curNote = owner.getNote(row, columnId);
                addAndMakeVisible(curNote);
            }
            
//            addAndMakeVisible(limeContent);
//            limeContent.setColour (TextButton::buttonColourId, Colours::red);
//            limeContent.setBounds(-10,-5,20,10);
        }
        
        void mouseDown (const MouseEvent& event) override
        {
            if (owner.getNote(row, columnId))
            {
                PianoRollNote* curNote = owner.getNote(row, columnId);
                owner.selectOneNote(curNote);
            }
            else
            {
                PianoRollNote* newNote = new PianoRollNote(row,columnId);
                owner.addNote(row, columnId, newNote);
                owner.selectOneNote(newNote);
                addAndMakeVisible(newNote);
            }
            repaint();
        }
        
        void setRowAndColumn (const int newRow, const int newColumn, bool isRowSelected)
        {
            if (row != newRow || columnId != newColumn) {
                removeAllChildren();
            }
            
            row = newRow;
            columnId = newColumn;
            
//            if (owner.getNote(row, columnId))
//            {
//                PianoRollNote* curNote = owner.getNote(row, columnId);
//                curNote->updateBounds(bd);
//            }
            if (owner.getNote(row, columnId))
            {
                PianoRollNote* curNote = owner.getNote(row, columnId);
                addAndMakeVisible(curNote);
            }
        }
        
        void paint (Graphics& g) override
        {
            g.setColour (Colours::grey);
            g.drawRect(g.getClipBounds().toFloat(), 0.5);
            //std::cout << "paint called " << row << ' ' << columnId << " - " << getBounds().getX() << ' ' << getBounds().getY() << std::endl;
//            if (owner.getNote(row, columnId))
//            {
//                PianoRollNote* curNote = owner.getNote(row, columnId);
//                curNote->updateBounds(g.getClipBounds());
//            }
        }
        
    private:
        TextButton limeContent;
        PianoRollTableListBox& owner;
        int row, columnId;
    };
};
