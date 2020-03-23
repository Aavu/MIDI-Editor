/*
  ==============================================================================

    PianoRollTableListModel.h
    Created: 26 Feb 2020 2:34:28pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

class NoteLayer: public TableListBoxModel, public NoteList, public SelectedNoteList, public Component
{
public:
    NoteLayer(): NoteList(this), SelectedNoteList(this)
    {
        oneColumnTable.getHeader().addColumn("notelayer", 1, Globals::noteWidth * Globals::tickNum, 30, -1,  TableHeaderComponent::ColumnPropertyFlags::notSortable);
        oneColumnTable.setModel(this);
        
        // hide horizontal scroll bars
        oneColumnTable.getViewport()->setScrollBarsShown(true, true, true, true);
        // hide header
        oneColumnTable.setHeaderHeight(0);
        oneColumnTable.setRowHeight(Globals::noteHeight);
        
        addAndMakeVisible(oneColumnTable);
    }
    void resized() override
    {
        auto area = getLocalBounds();
        oneColumnTable.setBounds(area);
    }
    int getNumRows() override
    {
        return Globals::midiNoteNum;
    }
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        g.fillAll (Colours::lightblue);
    }
    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override
    {
        // does not reach here
    }
    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected,
                                        Component* existingComponentToUpdate) override
    {
        auto* pianoRollRow = static_cast<RowComponent*> (existingComponentToUpdate);
        
        if (pianoRollRow == nullptr)
            pianoRollRow = new RowComponent (*this, rowNumber, columnId, Globals::tickNum);
        
        pianoRollRow->setRowAndColumn (rowNumber, 0, isRowSelected);
        
        return pianoRollRow;
    }
    
    bool keyPressed(const KeyPress &     key) override
    {
        //std::cout << "key code: " << key.getKeyCode() << std::endl;
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
    
    class RowComponent  : public Component
    {
    public:
        RowComponent (NoteLayer& lb, int row_n, int col_n, int tickNum) : owner (lb), row(row_n)
        {
            boxWidth = Globals::noteWidth;
            boxHeight = Globals::noteHeight;
            boxNum = tickNum;
            
            Array<PianoRollNote*> notesInRow = owner.getNotesByRow(row);
            for (int i = 0; i < notesInRow.size(); i++)
                addAndMakeVisible(notesInRow[i]);
        }
        
        ~RowComponent()
        {
        }
        
        void resized() override
        {
        }
        
        void setRowAndColumn (const int newRow, const int newColumn, bool isRowSelected)
        {
            if (row != newRow)
            {
                removeAllChildren();
                Array<PianoRollNote*> notesInRow = owner.getNotesByRow(newRow);
                for (int i = 0; i < notesInRow.size(); i++) {
                    addAndMakeVisible(notesInRow[i]);
                    float absOffset = notesInRow[i]->getOffset() * boxWidth;
                    float absLength = notesInRow[i]->getLength() * boxWidth;
                    notesInRow[i]->setBounds(absOffset, 0, absLength, boxHeight);
                }
            }
            row = newRow;
            selected = isRowSelected;
        }
        
        void paint (Graphics& g) override
        {
            g.setColour (Colours::grey);
            // draw boxes
            for (int i = 0; i < boxNum; i++)
                g.drawRect(1.F*i*boxWidth, 0.F, 1.F*boxWidth, 1.F*boxHeight, 0.5);
        }
        
        void mouseDown (const MouseEvent& event) override
        {
            auto* existingNote = static_cast<PianoRollNote*> (event.originalComponent);
            
            std::cout << "mouseDown called: " << row << std::endl;
            
            if (existingNote->ifInit() == false)   // create a new note
            {
                float offset = 1.F*event.getMouseDownX() / boxWidth;
                offset = (static_cast<int> (offset*2))/2.F; // quantize
                PianoRollNote* newNote = new PianoRollNote(row,offset);
                newNote->changePitch = [this] (PianoRollNote* note, int direction) { changePitch(note, direction); };
                
                owner.addNote(row, newNote);
                owner.selectOneNote(newNote);
                
                addAndMakeVisible(newNote);
                float absOffset = offset * boxWidth;
                float absLength = newNote->getLength() * boxWidth;
                newNote->setBounds(absOffset, 0, absLength, boxHeight);
                
                repaint();
            }
            else    // add the existing note to selected list
            {
                owner.selectOneNote(existingNote);
            }
        }
        
        void detachNote(PianoRollNote* noteToDetach)
        {
            removeChildComponent(noteToDetach);
            repaint();
        }
        
        void attachNote(PianoRollNote* noteToAttach)
        {
            addAndMakeVisible(noteToAttach);
            std::cout << "attachpitch rowcomp: " << row << std::endl;
            noteToAttach->changePitch = [this] (PianoRollNote* note, int direction) { changePitch(note, direction); };
            repaint();
        }
        
        void changePitch(PianoRollNote* noteToMove, int direction)
        {
            std::cout << "changepitch rowcomp: " << row << std::endl;
            detachNote(noteToMove);
            RowComponent* targetRow = static_cast<RowComponent*> (owner.oneColumnTable.getCellComponent(1,row+direction));
            targetRow->attachNote(noteToMove);
            owner.moveNote(row, noteToMove, direction);
        }
        
        void addToSelected(PianoRollNote* noteToAdd)
        {
            owner.selectOneNote(noteToAdd);
        }
    
        NoteLayer& owner;
        int row = -1;
        int boxWidth = -1;
        int boxHeight = -1;
        int boxNum = -1;
        
        bool selected = false;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RowComponent)
    };
    
private:
    TableListBox oneColumnTable;
};
