/*
  ==============================================================================

    ScrollablePianoRollComponent.h
    Created: 22 Feb 2020 6:06:34pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Globals.h"
#include <functional>

#include "Globals.h"
#include "PianoRollHeader.h"
#include "PianoRollNote.h"
#include "NoteLayer.h"
#include "CustomKeyboardComponent.h"

class OtherLookAndFeel;
class DragTextButton;
class RectNote;
class PianoRollListBox;

//==============================================================================
class OtherLookAndFeel : public LookAndFeel_V4
{
public:
    OtherLookAndFeel();

    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool, bool isButtonDown) override;
};

//==============================================================================
class PianoRollComponent   : public Component
{
public:
    PianoRollComponent();
    
    ~PianoRollComponent();

    void paint (Graphics& g) override;
    
    void resized() override;
    
    // This little function avoids a bit of code-duplication by adding a component to
    // our list as well as calling addAndMakeVisible on it..
    template <typename ComponentType>
    ComponentType* addToList (ComponentType* newComp);
    
private:
    OwnedArray<Component> components;
    
    OtherLookAndFeel otherLookAndFeel;
    
    PianoRollHeader                 *header;
    
    CustomKeyboardComponent         *keyboardComponent;
    
    NoteLayer                       *noteLayer;
    
    TextButton                      *sidebar;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRollComponent)
};

class ScrollablePianoRollComponent: public Component
{
public:

    ScrollablePianoRollComponent();

    void resized() override;

private:

    Viewport viewPort;
    PianoRollComponent cpn;
};
