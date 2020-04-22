/*
  ==============================================================================

    PianoRollNote.cpp
    Created: 23 Mar 2020 2:05:16am
    Author:  Jiawen Huang

  ==============================================================================
*/

#include "PianoRollNote.h"

PianoRollNote::PianoRollNote(int row_n, float offset_n, float length_n, int velocity_n, NoteMessage *noteMessage_n):
    m_iRow(row_n),
    m_fOffset(offset_n), m_fLength(length_n),
    m_iVelocity(velocity_n), m_pNoteMessage(noteMessage_n),
    m_pBorder(0)
{
    m_bInit = true;
    m_iBoxWidth = Globals::PianoRoll::initNoteWidth;
    m_iBoxHeight = Globals::PianoRoll::initNoteHeight;
    
    m_pBorder = new ResizableBorderComponent(this, NULL);
    addChildComponent(m_pBorder);
    m_pBorder->setBorderThickness (BorderSize<int> (0,0,0,1));
    m_pBorder->setBounds(this->getBounds());
    m_pBorder->setAlpha(0.1);
    m_pBorder->setVisible(true);
    
    m_pConstrainer = new ComponentBoundsConstrainer();
    m_pConstrainer->setMaximumHeight(m_iBoxHeight);
    std::cout << "create " << m_iRow << ' ' << offset_n << std::endl;
}

PianoRollNote::~PianoRollNote()
{
    if (m_pNoteMessage)
        delete m_pNoteMessage;
    if (m_pBorder)
        delete m_pBorder;
    if (m_pConstrainer)
        delete m_pConstrainer;
    std::cout << "delete " << m_iRow << ' ' << m_fOffset << std::endl;
}

void PianoRollNote::resized()
{
    // TODO: not working now
    if (m_pBorder)
    {
        m_pBorder->setBounds(0,0,getWidth(),getHeight());
        m_fLength = 1.F*getWidth() / m_iBoxWidth;
        //std::cout << m_fLength << std::endl;
        repaint();
    }
}

void PianoRollNote::mouseDown (const MouseEvent& event)
{
    // add to selectedNoteList
    getParentComponent()->mouseDown(event);
    m_pMyDragger.startDraggingComponent (this, event);
}

void PianoRollNote::mouseDrag (const MouseEvent& event)
{
    m_pMyDragger.dragComponent (this, event, m_pConstrainer);
    if (m_pConstrainer)
    {
        auto newBounds = getBoundsInParent();
        //std::cout << m_fOffset * m_iBoxWidth << ' '<< newBounds.getX() << std::endl;
        setBounds (newBounds);
        m_fOffset = 1.F * newBounds.getX() / m_iBoxWidth;
        m_pConstrainer->setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
    }
    if (event.getPosition().getY() < 0 && m_iRow > 0)
    {
        std::cout << "move up" << std::endl;
        changePitch(this, -1);
        m_iRow--;
    }
    else if (event.getPosition().getY() > m_iBoxHeight && m_iRow < Globals::PianoRoll::midiNoteNum-1)
    {
        std::cout << "move down" << std::endl;
        changePitch(this, 1);
        m_iRow++;
    }
    else
    {
        //std::cout << "stay" << std::endl;
    }
}

void PianoRollNote::mouseEnter(const MouseEvent& event)
{
    std::cout << "mouse enter: " << m_iRow << std::endl;
}

int PianoRollNote::getRow() { return m_iRow; }

float PianoRollNote::getOffset() { return m_fOffset; }
float PianoRollNote::getLength() { return m_fLength; }

NoteMessage* PianoRollNote::getNoteMessage() { return m_pNoteMessage; }

bool PianoRollNote::ifInit() { return m_bInit; }

void PianoRollNote::paintButton (Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.setColour (Colours::limegreen);
    g.fillRoundedRectangle(0, 0, m_fLength*m_iBoxWidth, m_iBoxHeight, 4);
    g.setColour (Colours::lime);
    g.drawRoundedRectangle(0, 0, m_fLength*m_iBoxWidth, m_iBoxHeight, 4, 1);
}

//------------------NoteList---------------------
NoteList::NoteList(SelectedNoteList *selected_n)
{
    m_pSelected = selected_n;
    m_pNoteList = new Array<PianoRollNote*> [Globals::PianoRoll::midiNoteNum];
    for (int i = 0; i < Globals::PianoRoll::midiNoteNum; i++) {
        m_pNoteList[i] = Array<PianoRollNote*>();
    }
}

NoteList::~NoteList()
{
    for (int i = 0; i < Globals::PianoRoll::midiNoteNum; i++) {
        for (int j = m_pNoteList[i].size() - 1; j >= 0 ; j--)
            if (getNote(i, j))
                deleteNote(i, j);
        m_pNoteList[i].clear();
    }
    delete [] m_pNoteList;
}

PianoRollNote* NoteList::getNote(int row, int idx)
{
    return m_pNoteList[row][idx];
}

Array<PianoRollNote*> NoteList::getNotesByRow(int row)
{
    return m_pNoteList[row];
}

void NoteList::addNote(int row, PianoRollNote* pianoRollNote)
{
    std::cout << "add Note to row: " << row << " offset: " << pianoRollNote->getOffset() << std::endl;

    m_pNoteList[row].add(pianoRollNote);
}

void NoteList::detachNote(int row, PianoRollNote* noteToMove)
{
    std::cout << "remove Note from row: " << row << " offset: " << noteToMove->getOffset() << std::endl;
    m_pNoteList[row].removeFirstMatchingValue(noteToMove);
}

void NoteList::moveNote(int row, PianoRollNote* noteToMove, int direction)
{
    detachNote(row, noteToMove);
    addNote(row+direction, noteToMove);
}

void NoteList::deleteNote(int row, int idx)
{
    PianoRollNote* noteToBeRemoved = getNote(row, idx);
    if (noteToBeRemoved)
    {
        m_pNoteList[row].remove(idx);
        delete noteToBeRemoved;
    }
}

void NoteList::deleteNote(PianoRollNote *noteToBeRemoved)
{
    int row = noteToBeRemoved->getRow();
    m_pNoteList[row].removeFirstMatchingValue(noteToBeRemoved);
    delete noteToBeRemoved;
}


//----------------SelectedNoteList--------------------

SelectedNoteList::SelectedNoteList(NoteList* noteList_n)
{
    m_pSelected = new Array<PianoRollNote*> ();
    m_pNoteList = noteList_n;
}
SelectedNoteList::~SelectedNoteList()
{
    m_pSelected->clear();
    delete m_pSelected;
}
void SelectedNoteList::selectOneNote(PianoRollNote* newNote)
{
    m_pSelected->clear();
    m_pSelected->add(newNote);
}

void SelectedNoteList::addSelectedNotes(PianoRollNote* newNote)
{
    m_pSelected->add(newNote);
}

void SelectedNoteList::removeSelectedNotes()
{
    PianoRollNote* noteToBeRemoved = 0;
    while (!m_pSelected->isEmpty())
    {
        noteToBeRemoved = m_pSelected->removeAndReturn(0);
        m_pNoteList->deleteNote(noteToBeRemoved);
    }
}
