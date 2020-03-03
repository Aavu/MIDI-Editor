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
#include "NoteLayer.h"

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
        
        noteLayer = addToList(new NoteLayer());
        noteLayer->setColour (TextButton::buttonColourId, Colours::grey);
        
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
        
        noteLayer->setBounds(sidebarWidth, headerFooterHeight, area.getWidth(), area.getHeight());
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
    
    NoteLayer                       *noteLayer;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRollWindow)
};
