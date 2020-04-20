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

    keyboardComponent = addToList(new CustomKeyboardComponent());
    
    noteLayer = addToList(new NoteLayer());
    noteLayer->setColour (TextButton::buttonColourId, Colours::grey);
    
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
    
    auto area_parent = getBoundsInParent();
    
    auto sidebarWidth = 80;
    keyboardComponent->setBounds (area.removeFromLeft (sidebarWidth));
    
    noteLayer->setBounds(sidebarWidth, 0, area.getWidth(), area.getHeight());

}

void PianoRollComponent::setPreview(bool ifPreview)
{
    preview = ifPreview;
    noteLayer->setPreview(ifPreview);
    keyboardComponent->setPreview(ifPreview);
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
    m_cpn.setSize(800,Globals::midiNoteNum*Globals::initNoteHeight);
    m_viewPort.setScrollBarsShown(true, false, true, false);
    m_viewPort.setViewedComponent(&m_cpn, false);
    
    addAndMakeVisible(&m_viewPort);

    setSize (800, 100);
    
    setPreview(false);
}

void ScrollablePianoRollComponent::resized()
{
    m_viewPort.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);

    m_cpn.resized();
}

void ScrollablePianoRollComponent::setPreview(bool ifPreview)
{
    m_preview = ifPreview;
    m_cpn.setPreview(ifPreview);
}
