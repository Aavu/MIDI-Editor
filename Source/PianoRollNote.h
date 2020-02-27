/*
  ==============================================================================

    PianoRollNote.h
    Created: 26 Feb 2020 2:23:00pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

// midi message associated with that note
// TODO: add members and functions later
class NodeMessage {
  // empty now
};

// TODO: fix dragging behavior
class PianoRollNote: public TextButton
{
public:
    PianoRollNote(int row_n, int column_n, int length_n = 1, int offset_n = 0, NodeMessage *noteMessage_n = 0):
        row(row_n), column(column_n), offset(offset_n), length(length_n), noteMessage(noteMessage_n)
    {}
    ~PianoRollNote()
    {
        if (noteMessage)
            delete noteMessage;
    }
    
    
//    void mouseDown (const MouseEvent& e)
//    {
//        myDragger.startDraggingComponent (this, e);
//    }
//
//    void mouseDrag (const MouseEvent& e)
//    {
//        myDragger.dragComponent (this, e, nullptr);
//    }
private:
    
    int                 row;                          // midi number (0~127)
    int                 column;                       // idx of column (0~inf)
    int                 offset;
    int                 length;                      // relative length (1 means 1 quarter note)
    
    NodeMessage         *noteMessage;
    
    // dragger
    ComponentDragger myDragger;
};
