/*
  ==============================================================================

    NoteLayer.cpp
    Created: 23 Mar 2020 12:58:35am
    Author:  Jiawen Huang

  ==============================================================================
*/

#include "NoteLayer.h"

NoteLayer::NoteLayer(): NoteList(this), SelectedNoteList(this)
{
    oneColumnTable.getHeader().addColumn("notelayer", 1, initNoteWidth * curTimeStamps, 30, -1,  TableHeaderComponent::ColumnPropertyFlags::notSortable);
    oneColumnTable.setModel(this);
    
    // hide horizontal scroll bars
    oneColumnTable.getViewport()->setScrollBarsShown(false, false, false, true);
    // hide header
    oneColumnTable.setHeaderHeight(0);
    oneColumnTable.setRowHeight(initNoteHeight);
    
    addAndMakeVisible(oneColumnTable);
}

void NoteLayer::resized()
{
	auto area = getLocalBounds();
    oneColumnTable.setBounds(area);
}

int NoteLayer::getNumRows()
{
	return Globals::midiNoteNum;
}

void NoteLayer::paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected)
{
    g.fillAll (Colours::lightblue);
}

void NoteLayer::paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/)
{
    // does not reach here
}

Component* NoteLayer::refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected,
                                        Component* existingComponentToUpdate)
{
    auto* pianoRollRow = static_cast<RowComponent*> (existingComponentToUpdate);
    
    if (pianoRollRow == nullptr)
        pianoRollRow = new RowComponent (*this, rowNumber, columnId, curTimeStamps, static_cast<int>(facNoteWidth*initNoteWidth), static_cast<int>(facNoteHeight*initNoteHeight), preview);
    
    pianoRollRow->setRowAndColumn (rowNumber, 0, isRowSelected);
    pianoRollRow->setPreview(preview);
    
    return pianoRollRow;
}

bool NoteLayer::keyPressed(const KeyPress & key)
{
    //std::cout << "key code: " << key.getKeyCode() << std::endl;
    if (key.getKeyCode() == 127) // delete key
    {
        removeSelectedNotes();
        repaint();
        return true;
    }
    else {
        return false;
    }
}

void NoteLayer::setPreview(bool ifPreview)
{
    preview = ifPreview;
    oneColumnTable.repaint();
}

NoteLayer::RowComponent::RowComponent (NoteLayer& lb, int row_n, int col_n, int tickNum, int curNoteWidth, int curNoteHeight, bool preview_n) : owner (lb), row(row_n)
{
    boxWidth = curNoteWidth;
    boxHeight = curNoteHeight;
    boxNum = tickNum;
    preview = preview_n;
    
    Array<PianoRollNote*> notesInRow = owner.getNotesByRow(row);
    for (int i = 0; i < notesInRow.size(); i++)
        addAndMakeVisible(notesInRow[i]);
}

void NoteLayer::RowComponent::setRowAndColumn (const int newRow, const int newColumn, bool isRowSelected)
{
    if (row != newRow)
    {
        removeAllChildren();
        Array<PianoRollNote*> notesInRow = owner.getNotesByRow(newRow);
        for (int i = 0; i < notesInRow.size(); i++) {
            addAndMakeVisible(notesInRow[i]);
            float absOffset = notesInRow[i]->getOffset() * boxWidth;
            float absLength = notesInRow[i]->getLength() * boxWidth;
            notesInRow[i]->setBounds(absOffset, 0, absLength, boxHeight);
        }
    }
    row = newRow;
    selected = isRowSelected;
}

void NoteLayer::RowComponent::paint (Graphics& g)
{
    g.setColour (Colours::grey);
    
    if (!preview)
    {
        // draw boxes
        for (int i = 0; i < boxNum; i++)
            g.drawRect(1.F*i*boxWidth, 0.F, 1.F*boxWidth, 1.F*boxHeight, 0.5);
    }
}

void NoteLayer::RowComponent::mouseDown (const MouseEvent& event)
{
    auto* existingNote = static_cast<PianoRollNote*> (event.originalComponent);
    
    std::cout << "mouseDown called: " << row << std::endl;
    
    if (existingNote->ifInit() == false)   // create a new note
    {
        float offset = 1.F*event.getMouseDownX() / boxWidth;
        offset = (static_cast<int> (offset*2))/2.F; // quantize
        PianoRollNote* newNote = new PianoRollNote(row,offset);
        newNote->changePitch = [this] (PianoRollNote* note, int direction) { changePitch(note, direction); };
        
        owner.addNote(row, newNote);
        owner.selectOneNote(newNote);
        
        addAndMakeVisible(newNote);
        float absOffset = offset * boxWidth;
        float absLength = newNote->getLength() * boxWidth;
        newNote->setBounds(absOffset, 0, absLength, boxHeight);
        
        repaint();
    }
    else    // add the existing note to selected list
    {
        owner.selectOneNote(existingNote);
    }
}

void NoteLayer::RowComponent::detachNote(PianoRollNote* noteToDetach)
{
    removeChildComponent(noteToDetach);
    repaint();
}

void NoteLayer::RowComponent::attachNote(PianoRollNote* noteToAttach)
{
    addAndMakeVisible(noteToAttach);
    std::cout << "attachpitch rowcomp: " << row << std::endl;
    noteToAttach->changePitch = [this] (PianoRollNote* note, int direction) { changePitch(note, direction); };
    repaint();
}

void NoteLayer::RowComponent::changePitch(PianoRollNote* noteToMove, int direction)
{
    std::cout << "changepitch rowcomp: " << row << std::endl;
    detachNote(noteToMove);
    RowComponent* targetRow = static_cast<RowComponent*> (owner.oneColumnTable.getCellComponent(1,row+direction));
    targetRow->attachNote(noteToMove);
    owner.moveNote(row, noteToMove, direction);
}

void NoteLayer::RowComponent::addToSelected(PianoRollNote* noteToAdd)
{
    owner.selectOneNote(noteToAdd);
}

void NoteLayer::RowComponent::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    // intercept mouseWheelMove event here, send the event to both owner (which later passes it to ScrollablePianoRollComponent)
    // and its parent (as it originally does)
    
    MouseWheelDetails wheel_h = wheel;
    MouseWheelDetails wheel_v = wheel;
    
    wheel_h.deltaY = 0;
    wheel_v.deltaX = 0;
    
    owner.mouseWheelMove (e.getEventRelativeTo (&owner), wheel_v);
    getParentComponent()->mouseWheelMove(e, wheel_h);
}

void NoteLayer::RowComponent::setPreview(bool ifPreview)
{
    preview = ifPreview;
}






