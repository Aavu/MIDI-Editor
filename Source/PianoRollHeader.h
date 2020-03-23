/*
  ==============================================================================

    PianoRollHeader.h
    Created: 26 Feb 2020 2:19:39pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class PianoRollHeader:   public TextButton,
                         public DragAndDropTarget
{
public:
    PianoRollHeader();
    
    bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/) override;
    
    void itemDragEnter (const SourceDetails& /*dragSourceDetails*/) override;
    
    void itemDragMove (const SourceDetails& /*dragSourceDetails*/) override;
    
    void itemDragExit (const SourceDetails& /*dragSourceDetails*/) override;
    
    void itemDropped (const SourceDetails& dragSourceDetails) override;

    void mouseEnter (const MouseEvent& event) override;

private:
    String message  { "Drag-and-drop some rows from the top-left box onto this component!\n\n"
        "You can also drag-and-drop files and text from other apps"};
    bool somethingIsBeingDraggedOver = false;
};
