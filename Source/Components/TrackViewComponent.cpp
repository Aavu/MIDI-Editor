/*
  ==============================================================================

    TrackViewComponent.cpp
    Created: 3 Mar 2020 8:32:54pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/
#include "TrackViewComponent.h"

TrackViewComponent::TrackViewComponent()
{
}

void TrackViewComponent::init(PlayerComponent* player) {
    // pianoroll list
    m_pianoRollListComp = new PianoRollListComponent();
    m_pianoRollListComp->init(player);

    addAndMakeVisible(*m_pianoRollListComp);
}

TrackViewComponent::~TrackViewComponent() {
    delete m_pianoRollListComp;
}

void TrackViewComponent::paint(Graphics &g) {
    g.fillAll (Colours::lightgrey);
}

void TrackViewComponent::resized() {
    auto area = getLocalBounds();
    m_pianoRollListComp->setBounds(area);
}

void TrackViewComponent::addTrack(int numTimeStampsForPianoRoll) {
    m_pianoRollListComp->setTrack(numTimeStampsForPianoRoll);
    resized();
}

void TrackViewComponent::setTimeFormat(int timeFormat)
{
    // currently only allow time stamp ticks - see the documentation of Class MidiFile
    assert(timeFormat > 0);
    
    m_pianoRollListComp->setTimeFormat(timeFormat);
}

void TrackViewComponent::convertMidiMessageSequence(int trackIdx, const MidiMessageSequence *message)
{
    // pass to m_pianoRollListComp
    m_pianoRollListComp->convertMidiMessageSequence(trackIdx, message);
}
