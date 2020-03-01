/*
  ==============================================================================

    PianoRollComponent.h
    Created: 22 Feb 2020 6:06:34pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "Globals.h"
#include "PianoRollHeader.h"
#include "PianoRollNote.h"
#include "PianoRollTableListModel.h"

class OtherLookAndFeel;
class DragTextButton;
class RectNote;
class PianoRollListBox;

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
        
        header = addToList(new PianoRollHeader());
        header->setColour (TextButton::buttonColourId, Colours::cornflowerblue);
        header->setButtonText ("Ticks");
        
        sidebar = addToList(new TextButton());
        sidebar->setColour (TextButton::buttonColourId, Colours::grey);
        sidebar->setButtonText ("Piano Keys");
        
        // set border
        table = addToList(new PianoRollTableListBox("D+D source", nullptr));
        table->setColour (ListBox::outlineColourId, Colours::grey);
        table->setOutlineThickness (2);
        
        // add columns
        for (int i = 1; i <= Globals::tickNum; i++)
            table->getHeader().addColumn(String(i), i, Globals::noteWidth, 30, -1,  TableHeaderComponent::ColumnPropertyFlags::notResizableOrSortable);
        
        // hide horizontal scroll bars
        table->getViewport()->setScrollBarsShown(true, false, true, true);
        // hide header
        table->setHeaderHeight(0);
        
        setSize (800, 400);
    }
    
    ~PianoRollWindow()
    {
        setLookAndFeel (nullptr);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::darkgreen);
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        
        auto sidebarWidth = 80;
        sidebar->setBounds (area.removeFromLeft (sidebarWidth));
        
        auto headerFooterHeight = 20;
        header->setBounds (area.removeFromTop    (headerFooterHeight));
        
        table->setBounds(sidebarWidth, headerFooterHeight, area.getWidth(), area.getHeight());
        table->setRowHeight(Globals::noteHeight);
    }
    
    // This little function avoids a bit of code-duplication by adding a component to
    // our list as well as calling addAndMakeVisible on it..
    template <typename ComponentType>
    ComponentType* addToList (ComponentType* newComp)
    {
        components.add (newComp);
        addAndMakeVisible (newComp);
        return newComp;
    }
    
private:
    OwnedArray<Component> components;
    
    OtherLookAndFeel otherLookAndFeel;
    
    PianoRollHeader                 *header;
    TextButton                      *sidebar;
    
    PianoRollTableListBox           *table;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRollWindow)
};
