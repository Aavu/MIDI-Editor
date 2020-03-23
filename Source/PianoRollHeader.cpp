/*
  ==============================================================================

    PianoRollHeader.cpp
    Created: 23 Mar 2020 12:59:28am
    Author:  Jiawen Huang

  ==============================================================================
*/

#include "PianoRollHeader.h"

PianoRollHeader::PianoRollHeader(){};
    
bool PianoRollHeader::isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/)
{
    // normally you'd check the sourceDescription value to see if it's the
    // sort of object that you're interested in before returning true, but for
    // the demo, we'll say yes to anything..
    return true;
}

void PianoRollHeader::itemDragEnter (const SourceDetails& /*dragSourceDetails*/)
{
    somethingIsBeingDraggedOver = true;
    repaint();
}

void PianoRollHeader::itemDragMove (const SourceDetails& /*dragSourceDetails*/)
{
}

void PianoRollHeader::itemDragExit (const SourceDetails& /*dragSourceDetails*/)
{
    somethingIsBeingDraggedOver = false;
    repaint();
}

void PianoRollHeader::itemDropped (const SourceDetails& dragSourceDetails)
{
    message = "Items dropped: " + dragSourceDetails.description.toString();
    
    somethingIsBeingDraggedOver = false;
    repaint();
}
void PianoRollHeader::mouseEnter (const MouseEvent& event)
{
    
}