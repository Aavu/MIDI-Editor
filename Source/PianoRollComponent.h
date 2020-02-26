/*
  ==============================================================================

    PianoRollComponent.h
    Created: 26 Feb 2020 2:32:32pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

class PianoRollTableListBoxModel;

class PianoRollComponent  : public Component
{
public:
    PianoRollComponent (PianoRollTableListBoxModel* td)  : owner (td)
    {
        isSelected = false;
    }
    
    void mouseDown (const MouseEvent& event) override
    {
        // single click on the label should simply select the row
        //owner.table.selectRowsBasedOnModifierKeys (row, event.mods, false);
        std::cout << row << ' ' << columnId << std::endl;
        if (isSelected)
            isSelected = false;
        else
            isSelected = true;
        repaint();
    }
    
    // Our demo code will call this when we may need to update our contents
    void setRowAndColumn (const int newRow, const int newColumn)
    {
        row = newRow;
        columnId = newColumn;
    }
    
    void paint (Graphics& g) override
    {
        g.setColour (Colours::grey);
        
        //g.setFont (height * 0.7f);
        
        // g.drawText (String(rowNumber), 2, 0, width - 4, height, Justification::centredLeft, true);
        
        g.drawRect(g.getClipBounds().toFloat(), 0.5);
        
        g.setColour (Colours::lime);
        if (isSelected) {
            g.fillRect(g.getClipBounds().toFloat());
        }
        
        g.setColour (Colours::black);
    }
    
private:
    PianoRollTableListBoxModel* owner;
    int row, columnId;
    bool isSelected;
};
