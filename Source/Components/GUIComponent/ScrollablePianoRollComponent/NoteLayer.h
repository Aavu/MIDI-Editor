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
    
    NoteLayer(int numTimeStamps = 30);
    
    void resized() override;
    
    int getNumRows() override;
    
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override;

    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override;
    
    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected,
                                        Component* existingComponentToUpdate) override;
    
    bool keyPressed(const KeyPress & key) override;
    
    void addNoteToRow(PianoRollNote *newNote);
    
    void addEmptyColumns(int numColumnsToAdd);
    
    void highlightRow(int row);
    
    void setPreview(bool ifPreview);
    
    class RowComponent  : public Component
    {
    public:
        RowComponent (NoteLayer& lb, int row_n, int col_n, int tickNum, int curNoteWidth, int curNoteHeight, bool preview_n);
        
        void setRowAndColumn (const int newRow, const int newColumn, bool isRowSelected);
        
        void paint (Graphics& g) override;
        
        void mouseDown (const MouseEvent& event) override;
        
        void mouseDoubleClick (const MouseEvent& event) override;
        
        void addNote(PianoRollNote* newNote);
        
        void detachNote(PianoRollNote* noteToDetach);
        
        void attachNote(PianoRollNote* noteToAttach);
        
        void changePitch(PianoRollNote* noteToMove, int direction);
        
        void addToSelected(PianoRollNote* noteToAdd);

        void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel) override;
        
        void mouseEnter(const MouseEvent& event) override;
        
        void setPreview(bool ifPreview);
    
        NoteLayer& m_Owner;
        int m_iRow = -1;
        int m_iBoxWidth = -1;
        int m_iBoxHeight = -1;
        int m_iBoxNum = -1;
        
        bool m_bSelected = false;
        bool m_bPreview = true;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RowComponent)
    };
    
private:
    
    TableListBox oneColumnTable;
    
    int m_iInitNoteWidth = Globals::PianoRoll::initNoteWidth;
    int m_iInitNoteHeight = Globals::PianoRoll::initNoteHeight;
    int m_iCurTimeStamps = Globals::PianoRoll::initTimeStamps;
    
    float m_fFacNoteWidth = 1.F;
    float m_fFacNoteHeight = 1.F;
    
    bool m_bPreview = true;
};
