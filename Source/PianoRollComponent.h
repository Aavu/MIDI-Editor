/*
  ==============================================================================

    PianoRollComponent.h
    Created: 22 Feb 2020 6:06:34pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "PianoRollHeader.h"
#include "PianoRollNote.h"
#include "PianoRollTableListModel.h"

class OtherLookAndFeel;
class DragTextButton;
class RectNote;
class PianoRollListBox;

// init setting
const int midiNoteNum = 128;
const int tickNum = 40;
const int noteHeight = 10;
const int noteWidth = 40;

//==============================================================================
class OtherLookAndFeel : public LookAndFeel_V4
{
public:
    OtherLookAndFeel()
    {
    }
    
    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool, bool isButtonDown) override
    {
        auto buttonArea = button.getLocalBounds();
        
        g.setColour (backgroundColour);
        g.fillRect (buttonArea);
    }
    
};

//==============================================================================
class PianoRollWindow   : public Component
{
public:
    PianoRollWindow()
    {
        setLookAndFeel (&otherLookAndFeel);
        
        header.setColour (TextButton::buttonColourId, Colours::cornflowerblue);
        header.setButtonText ("Ticks");
        addAndMakeVisible (header);
        
        sidebar.setColour (TextButton::buttonColourId, Colours::grey);
        sidebar.setButtonText ("Piano Keys");
        addAndMakeVisible (sidebar);
        
        // set border
        table.setColour (ListBox::outlineColourId, Colours::grey);
        table.setOutlineThickness (2);
        addAndMakeVisible(table);
        
        // pass noteList and selectedNoteList to model
        // model needs to check noteList when drawing
        // model respond to mouse events
        pianoRollTableListBoxModel.setNoteList(&noteList);
        pianoRollTableListBoxModel.setSelectedNoteList(&selectedNoteList);
        
        // pass noteList and selectedNoteList to table
        // table respond to mouse events
        table.setModel (&pianoRollTableListBoxModel);
        table.setNoteList(&noteList);
        table.setSelectedNoteList(&selectedNoteList);
        
        // add columns
        for (int i = 1; i <= tickNum; i++)
            table.getHeader().addColumn(String(i), i, noteWidth, 30, -1,  TableHeaderComponent::ColumnPropertyFlags::notResizableOrSortable);
        
        // hide scroll bars
        table.getViewport()->setScrollBarsShown(true, false, true, true);
        
        // hide header
        table.setHeaderHeight(0);
        
        setSize (800, 400);
    }
    
    ~PianoRollWindow()
    {
        setLookAndFeel (nullptr);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::darkgrey);
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        
        auto sidebarWidth = 80;
        sidebar.setBounds (area.removeFromLeft (sidebarWidth));
        
        auto headerFooterHeight = 20;
        header.setBounds (area.removeFromTop    (headerFooterHeight));
        
        table.setBounds(sidebarWidth, headerFooterHeight, area.getWidth(), area.getHeight());
        table.setRowHeight(noteHeight);
    }
    
//    bool keyPressed(const KeyPress &     key) override
//    {
//        if (key.getKeyCode() == 127) // delete key
//        {
//            selectedNotes.removeNotes();
//            return true;
//        }
//        else {
//            return false;
//        }
//    }
    
private:
    
    OtherLookAndFeel otherLookAndFeel;
    
    PianoRollHeader header;
    TextButton sidebar;
    
    PianoRollTableListBox table  { "D+D source", nullptr };
    PianoRollTableListBoxModel pianoRollTableListBoxModel;
    
    NoteList                            noteList;
    SelectedNoteList                    selectedNoteList {&noteList};
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRollWindow)
};
