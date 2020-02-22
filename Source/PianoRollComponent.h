/*
  ==============================================================================

    PianoRollWindow.h
    Created: 22 Feb 2020 6:06:34pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once


class OtherLookAndFeel;
class DragTextButton;
class RectNote;
class PianoRollListBox;

// init setting
const int midiNoteNum = 128;
const int tickNum = 20;
const int noteHeight = 10;
const int noteWidth = 40;

//==============================================================================
class OtherLookAndFeel : public LookAndFeel_V4
{
public:
    OtherLookAndFeel()
    {
    }
    
    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool, bool isButtonDown) override
    {
        auto buttonArea = button.getLocalBounds();
        
        g.setColour (backgroundColour);
        g.fillRect (buttonArea);
    }
    
};

// TODO: fix dragging behavior
class DragTextButton: public TextButton
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

class PianoRollTableListBoxModel: public TableListBoxModel
{
public:
    // The following methods implement the necessary virtual functions from ListBoxModel,
    // telling the listbox how many rows there are, painting them, etc.
    int getNumRows() override
    {
        return midiNoteNum;
    }
    
    // This is overloaded from TableListBoxModel, and should fill in the background of the whole row
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        g.fillAll (Colours::lightblue);
    }
    
    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override
    {
        // control does not reach here because all the components are customized
        
        g.setColour (Colours::grey);
        
        g.setFont (height * 0.7f);
        
        g.drawText (String(rowNumber), 2, 0, width - 4, height, Justification::centredLeft, true);
        
        g.drawRect(g.getClipBounds().toFloat(), 1.5);
        
        g.setColour (Colours::black);
        
    }
    
    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool /*isRowSelected*/,
                                        Component* existingComponentToUpdate) override
    {
        // The other columns are editable text columns, for which we use the custom Label component
        auto* pianoRollBox = static_cast<PianoRollComponent*> (existingComponentToUpdate);
        
        // same as above...
        if (pianoRollBox == nullptr)
            pianoRollBox = new PianoRollComponent (*this);
        
        pianoRollBox->setRowAndColumn (rowNumber, columnId);
        return pianoRollBox;
    }
    
    //    var getDragSourceDescription (const SparseSet<int>& selectedRows) override
    //    {
    //        // for our drag description, we'll just make a comma-separated list of the selected row
    //        // numbers - this will be picked up by the drag target and displayed in its box.
    //        StringArray rows;
    //
    //        for (int i = 0; i < selectedRows.size(); ++i)
    //            rows.add (String (selectedRows[i] + 1));
    //
    //        return rows.joinIntoString (", ");
    //    }
    
private:
    class PianoRollComponent  : public Component
    {
    public:
        PianoRollComponent (PianoRollTableListBoxModel& td)  : owner (td)
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
            if (isSelected)
                g.fillRect(g.getClipBounds().toFloat());
            
            g.setColour (Colours::black);
        }
        
    private:
        PianoRollTableListBoxModel& owner;
        int row, columnId;
        bool isSelected;
    };
};

//==============================================================================
class PianoRollComponent   : public Component, public DragAndDropContainer
{
public:
    PianoRollComponent()
    {
        setLookAndFeel (&otherLookAndFeel);
        
        
        header.setColour (TextButton::buttonColourId, Colours::cornflowerblue);
        header.setButtonText ("Ticks");
        addAndMakeVisible (header);
        
        sidebar.setColour (TextButton::buttonColourId, Colours::grey);
        sidebar.setButtonText ("Piano Keys");
        addAndMakeVisible (sidebar);
        
        limeContent.setColour (TextButton::buttonColourId, Colours::lime);
        addAndMakeVisible (limeContent);
        
        // set border
        table.setColour (ListBox::outlineColourId, Colours::grey);
        table.setOutlineThickness (2);
        addAndMakeVisible(table);
        
        table.setModel (&pianoRollTableListBoxModel);
        
        // add columns
        for (int i = 1; i <= tickNum; i++)
            table.getHeader().addColumn(String(i), i, noteWidth, 30, -1,  TableHeaderComponent::ColumnPropertyFlags::notResizableOrSortable);
        
        // hide scroll bars
        table.getViewport()->setScrollBarsShown(true, false, true, true);
        
        // hide header
        table.setHeaderHeight(0);
        
        setSize (800, 400);
    }
    
    ~PianoRollComponent()
    {
        setLookAndFeel (nullptr);
    }
    
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::darkgrey);
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        
        auto sidebarWidth = 80;
        sidebar.setBounds (area.removeFromLeft (sidebarWidth));
        
        auto headerFooterHeight = 20;
        header.setBounds (area.removeFromTop    (headerFooterHeight));
        
        limeContent.setBounds       (sidebarWidth, headerFooterHeight, noteWidth, noteHeight);
        
        table.setBounds(sidebarWidth, headerFooterHeight, area.getWidth(), area.getHeight());
        table.setRowHeight(noteHeight);
    }
    
private:
    
    class DropTextButton:   public TextButton,
    public DragAndDropTarget
    {
    public:
        DropTextButton(){};
        
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
    
    OtherLookAndFeel otherLookAndFeel;
    DropTextButton header;
    TextButton sidebar;
    
    DragTextButton limeContent;
    
    TableListBox table  { "D+D source", nullptr };
    PianoRollTableListBoxModel pianoRollTableListBoxModel;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRollComponent)
};
