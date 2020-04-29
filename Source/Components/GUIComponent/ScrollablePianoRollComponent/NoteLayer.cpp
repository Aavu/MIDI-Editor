/*
  ==============================================================================

    NoteLayer.cpp
    Created: 23 Mar 2020 12:58:35am
    Author:  Jiawen Huang

  ==============================================================================
*/

#include "NoteLayer.h"

NoteLayer::NoteLayer(int numTimeStamps): NoteList(this), SelectedNoteList(this)
{
    m_iCurTimeStamps = numTimeStamps;
    
    oneColumnTable.getHeader().addColumn("notelayer", 1, m_iInitNoteWidth * m_iCurTimeStamps, 30, -1,  TableHeaderComponent::ColumnPropertyFlags::notSortable);
    oneColumnTable.setModel(this);
    
    // hide horizontal scroll bars
    oneColumnTable.getViewport()->setScrollBarsShown(false, false, false, true);
    // hide header
    oneColumnTable.setHeaderHeight(0);
    oneColumnTable.setRowHeight(m_iInitNoteHeight);
    
    addAndMakeVisible(oneColumnTable);
}

void NoteLayer::resized()
{
	auto area = getLocalBounds();
    oneColumnTable.setBounds(area);
}

int NoteLayer::getNumRows()
{
    return Globals::PianoRoll::midiNoteNum;
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
        pianoRollRow = new RowComponent (*this, rowNumber, columnId, m_iCurTimeStamps, static_cast<int>(m_fFacNoteWidth*m_iInitNoteWidth), static_cast<int>(m_fFacNoteHeight*m_iInitNoteHeight), m_bPreview);
    
    pianoRollRow->setRowAndColumn (rowNumber, 0, isRowSelected);
    pianoRollRow->setPreview(m_bPreview);
    
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

void NoteLayer::addNoteToRow(PianoRollNote *newNote)
{
    int row = newNote->getRow();
    
    auto* pianoRollRow = static_cast<RowComponent*>(oneColumnTable.getCellComponent(1, row));
    
    pianoRollRow->addNote(newNote);
}

void NoteLayer::addEmptyColumns(int numColumnsToAdd)
{
    m_iCurTimeStamps += numColumnsToAdd;
    oneColumnTable.getHeader().setColumnWidth(1, m_iInitNoteWidth * m_iCurTimeStamps);
}

void NoteLayer::highlightRow(int row)
{
    // call
}

void NoteLayer::setPreview(bool ifPreview)
{
    m_bPreview = ifPreview;
    oneColumnTable.repaint();
}

int NoteLayer::getViewPositionX()
{
    return oneColumnTable.getViewport()->getViewPositionX();
}

int NoteLayer::getBoxWidth()
{
    return static_cast<int>(m_fFacNoteWidth*m_iInitNoteWidth);
}

int NoteLayer::getBoxHeight()
{
    return static_cast<int>(m_fFacNoteHeight*m_iInitNoteHeight);
}

int NoteLayer::getCanvasWidth()
{
    return m_fFacNoteWidth * m_iInitNoteWidth * m_iCurTimeStamps;
}



NoteLayer::RowComponent::RowComponent (NoteLayer& lb, int row_n, int col_n, int tickNum, int curNoteWidth, int curNoteHeight, bool preview_n) : m_Owner (lb), m_iRow(row_n)
{
    m_iBoxWidth = curNoteWidth;
    m_iBoxHeight = curNoteHeight;
    m_iBoxNum = tickNum;
    m_bPreview = preview_n;
    
    Array<PianoRollNote*> notesInRow = m_Owner.getNotesByRow(m_iRow);
    for (int i = 0; i < notesInRow.size(); i++)
        addAndMakeVisible(notesInRow[i]);
}

void NoteLayer::RowComponent::setRowAndColumn (const int newRow, const int newColumn, bool isRowSelected)
{
    if (m_iRow != newRow)
    {
        removeAllChildren();
        Array<PianoRollNote*> notesInRow = m_Owner.getNotesByRow(newRow);
        for (int i = 0; i < notesInRow.size(); i++) {
            addAndMakeVisible(notesInRow[i]);
            float absOffset = notesInRow[i]->getOffset() * m_iBoxWidth;
            float absLength = notesInRow[i]->getLength() * m_iBoxWidth;
            notesInRow[i]->setBounds(absOffset, 0, absLength, m_iBoxHeight);
        }
    }
    m_iRow = newRow;
    m_bSelected = isRowSelected;
}

void NoteLayer::RowComponent::paint (Graphics& g)
{
    g.setColour (Colours::grey);
    
    if (!m_bPreview)
    {
        // draw boxes
        for (int i = 0; i < m_iBoxNum; i++)
            g.drawRect(1.F*i*m_iBoxWidth, 0.F, 1.F*m_iBoxWidth, 1.F*m_iBoxHeight, 0.5);
    }
}

void NoteLayer::RowComponent::mouseDown (const MouseEvent& event)
{
    auto* existingNote = static_cast<PianoRollNote*> (event.originalComponent);
    
    std::cout << "mouseDown called: " << m_iRow << std::endl;
    
    if (existingNote->ifInit() == false)   // create a new note
    {
        // do nothing
    }
    else    // add the existing note to selected list
    {
        m_Owner.selectOneNote(existingNote);
    }
}

void NoteLayer::RowComponent::mouseDoubleClick (const MouseEvent& event)
{
    auto* existingNote = static_cast<PianoRollNote*> (event.originalComponent);
    
    std::cout << "mouseDown called: " << m_iRow << std::endl;
    
    if (existingNote->ifInit() == false)   // create a new note
    {
        float offset = 1.F*event.getMouseDownX() / m_iBoxWidth;
        offset = (static_cast<int> (offset*2))/2.F; // quantize
        PianoRollNote* newNote = new PianoRollNote(nullptr, m_iRow,offset); // CHANGE
        
        addNote(newNote);
        m_Owner.selectOneNote(newNote);
    }
    else    // add the existing note to selected list
    {
        m_Owner.selectOneNote(existingNote);
    }
}

void NoteLayer::RowComponent::addNote(PianoRollNote* newNote)
{
    newNote->changePitch = [this] (PianoRollNote* note, int direction) { changePitch(note, direction); };
    newNote->hightlightRow = [this] () { m_Owner.highlightRow(m_iRow); };
    
    m_Owner.addNote(m_iRow, newNote);
    
    addAndMakeVisible(newNote);
    float absOffset = newNote->getOffset() * m_iBoxWidth;
    float absLength = newNote->getLength() * m_iBoxWidth;
    newNote->setBounds(absOffset, 0, absLength, m_iBoxHeight);
    
    repaint();
}

void NoteLayer::RowComponent::detachNote(PianoRollNote* noteToDetach)
{
    removeChildComponent(noteToDetach);
    repaint();
}

void NoteLayer::RowComponent::attachNote(PianoRollNote* noteToAttach)
{
    addAndMakeVisible(noteToAttach);
    std::cout << "attachpitch rowcomp: " << m_iRow << std::endl;
    noteToAttach->changePitch = [this] (PianoRollNote* note, int direction) { changePitch(note, direction); };
    repaint();
}

void NoteLayer::RowComponent::changePitch(PianoRollNote* noteToMove, int direction)
{
    std::cout << "changepitch rowcomp: " << m_iRow << std::endl;
    detachNote(noteToMove);
    RowComponent* targetRow = static_cast<RowComponent*> (m_Owner.oneColumnTable.getCellComponent(1,m_iRow+direction));
    targetRow->attachNote(noteToMove);
    m_Owner.moveNote(m_iRow, noteToMove, direction);
}

void NoteLayer::RowComponent::addToSelected(PianoRollNote* noteToAdd)
{
    m_Owner.selectOneNote(noteToAdd);
}

void NoteLayer::RowComponent::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    // intercept mouseWheelMove event here, send the event to both m_Owner (which later passes it to ScrollablePianoRollComponent)
    // and its parent (as it originally does)
    
    MouseWheelDetails wheel_h = wheel;
    MouseWheelDetails wheel_v = wheel;
    
    wheel_h.deltaY = 0;
    wheel_v.deltaX = 0;
    
    m_Owner.mouseWheelMove (e.getEventRelativeTo (&m_Owner), wheel_v);
    getParentComponent()->mouseWheelMove(e, wheel_h);
}

void NoteLayer::RowComponent::mouseEnter(const MouseEvent& event)
{
    m_Owner.highlightRow(m_iRow);
}

void NoteLayer::RowComponent::setPreview(bool ifPreview)
{
    m_bPreview = ifPreview;
}
