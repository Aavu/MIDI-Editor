/*
  ==============================================================================

    NoteLayer.h
    Created: 26 Feb 2020 2:34:28pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PianoRollNote.h"

class NoteLayer: public TableListBoxModel, public NoteList, public SelectedNoteList, public Component
{
public:
    
    NoteLayer();
    
    void resized() override;
    
    int getNumRows() override;
    
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override;

    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override;
    
    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected,
                                        Component* existingComponentToUpdate) override;
    
    bool keyPressed(const KeyPress & key) override;
    
    class RowComponent  : public Component
    {
    public:
        RowComponent (NoteLayer& lb, int row_n, int col_n, int tickNum);
        
        void setRowAndColumn (const int newRow, const int newColumn, bool isRowSelected);
        
        void paint (Graphics& g) override;
        
        void mouseDown (const MouseEvent& event) override;
        
        void detachNote(PianoRollNote* noteToDetach);
        
        void attachNote(PianoRollNote* noteToAttach);
        
        void changePitch(PianoRollNote* noteToMove, int direction);
        
        void addToSelected(PianoRollNote* noteToAdd);
    
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
