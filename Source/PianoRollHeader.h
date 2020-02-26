/*
  ==============================================================================

    PianoRollHeader.h
    Created: 26 Feb 2020 2:19:39pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

class PianoRollHeader:   public TextButton,
                         public DragAndDropTarget
{
public:
    PianoRollHeader(){};
    
    //        void mouseDown (const MouseEvent& e) override
    //        {
    //            std::cout << e.getMouseDownX() << ' ' << e.getMouseDownY() << std::endl;
    //            std::cout << ' ' << std::endl;
    //        }
    
    // These methods implement the DragAndDropTarget interface, and allow our component
    // to accept drag-and-drop of objects from other JUCE components..
    
    bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/) override
    {
        // normally you'd check the sourceDescription value to see if it's the
        // sort of object that you're interested in before returning true, but for
        // the demo, we'll say yes to anything..
        return true;
    }
    
    void itemDragEnter (const SourceDetails& /*dragSourceDetails*/) override
    {
        somethingIsBeingDraggedOver = true;
        repaint();
    }
    
    void itemDragMove (const SourceDetails& /*dragSourceDetails*/) override
    {
    }
    
    void itemDragExit (const SourceDetails& /*dragSourceDetails*/) override
    {
        somethingIsBeingDraggedOver = false;
        repaint();
    }
    
    void itemDropped (const SourceDetails& dragSourceDetails) override
    {
        message = "Items dropped: " + dragSourceDetails.description.toString();
        
        somethingIsBeingDraggedOver = false;
        repaint();
    }
private:
    String message  { "Drag-and-drop some rows from the top-left box onto this component!\n\n"
        "You can also drag-and-drop files and text from other apps"};
    bool somethingIsBeingDraggedOver = false;
};
