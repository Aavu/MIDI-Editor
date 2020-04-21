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

    m_pKeyboardComponent = addToList(new CustomKeyboardComponent());
    
    m_pNoteLayer = addToList(new NoteLayer());
    m_pNoteLayer->setColour (TextButton::buttonColourId, Colours::grey);
    
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
    m_pKeyboardComponent->setBounds (area.removeFromLeft (sidebarWidth));
    
    m_pNoteLayer->setBounds(sidebarWidth, 0, area.getWidth(), area.getHeight());

}

void PianoRollComponent::setPreview(bool ifPreview)
{
    m_Preview = ifPreview;
    m_pNoteLayer->setPreview(ifPreview);
    m_pKeyboardComponent->setPreview(ifPreview);
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
    m_Cpn.setSize(1000,Globals::PianoRoll::midiNoteNum*Globals::PianoRoll::initNoteHeight);
    m_ViewPort.setScrollBarsShown(false, false, true, false);
    m_ViewPort.setViewedComponent(&m_Cpn, false);
    
    addAndMakeVisible(&m_ViewPort);

    setSize (1000, 100);
    
    setPreview(false);
}

void ScrollablePianoRollComponent::resized()
{
    auto area = getLocalBounds();
    
    m_ViewPort.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
    
    // change pianoroll visible area width
    m_Cpn.setSize(area.getWidth(),Globals::PianoRoll::midiNoteNum*Globals::PianoRoll::initNoteHeight);

}

void ScrollablePianoRollComponent::setPreview(bool ifPreview)
{
    m_bPreview = ifPreview;
    m_Cpn.setPreview(ifPreview);
}
