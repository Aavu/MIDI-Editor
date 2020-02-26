/*
  ==============================================================================

    PianoRollNote.h
    Created: 26 Feb 2020 2:23:00pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

// TODO: fix dragging behavior
class PianoRollNote: public TextButton
{
    ComponentDragger myDragger;
    void mouseDown (const MouseEvent& e)
    {
        myDragger.startDraggingComponent (this, e);
    }
    
    void mouseDrag (const MouseEvent& e)
    {
        myDragger.dragComponent (this, e, nullptr);
    }
    
};
