/*
  ==============================================================================

    ScrollablePianoRollComponent.cpp
    Created: 23 Mar 2020 2:05:36am
    Author:  Jiawen Huang

  ==============================================================================
*/

#include "ScrollablePianoRollComponent.h"

//-------------OtherLookAndFeel----------------

OtherLookAndFeel::OtherLookAndFeel()
{
}

void OtherLookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                           bool, bool isButtonDown)
{
    auto buttonArea = button.getLocalBounds();
    
    g.setColour (backgroundColour);
    g.fillRect (buttonArea);
}

//--------------PianoRollComponent---------------

PianoRollComponent::PianoRollComponent()
{
    setLookAndFeel (&otherLookAndFeel);
    
//    header = addToList(new PianoRollHeader());
//    header->setColour (TextButton::buttonColourId, Colours::cornflowerblue);
//    header->setButtonText ("Ticks");
    
//    sidebar = addToList(new TextButton());
//    sidebar->setColour (TextButton::buttonColourId, Colours::grey);
//    sidebar->setButtonText ("Piano Keys");
    keyboardComponent = addToList(new CustomKeyboardComponent());
    
    noteLayer = addToList(new NoteLayer());
    noteLayer->setColour (TextButton::buttonColourId, Colours::grey);
    
    //setSize (800, 400);
}

PianoRollComponent::~PianoRollComponent()
{
    setLookAndFeel (nullptr);
}

void PianoRollComponent::paint (Graphics& g)
{
    g.fillAll (Colours::darkgreen);
}

void PianoRollComponent::resized()
{
    auto area = getLocalBounds();
    
//    auto headerFooterHeight = 20;
//    header->setBounds (area.removeFromTop    (headerFooterHeight));
    
    auto sidebarWidth = 80;
    //sidebar->setBounds (area.removeFromLeft (sidebarWidth));
    keyboardComponent->setBounds (area.removeFromLeft (sidebarWidth));
    
    noteLayer->setBounds(sidebarWidth, 0, area.getWidth(), area.getHeight());
}

template <typename ComponentType>
ComponentType* PianoRollComponent::addToList (ComponentType* newComp)
{
    components.add (newComp);
    addAndMakeVisible (newComp);
    return newComp;
}

//------------------------ScrollablePianoRollComponent--------------------------

ScrollablePianoRollComponent::ScrollablePianoRollComponent()
{
    cpn.setSize(800,Globals::midiNoteNum*Globals::initNoteHeight);
    viewPort.setScrollBarsShown(true, false, true, false);
    viewPort.setViewedComponent(&cpn, false);
    
    addAndMakeVisible(&viewPort);

    setSize (800, 400);
}

void ScrollablePianoRollComponent::resized()
{
    viewPort.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
}
