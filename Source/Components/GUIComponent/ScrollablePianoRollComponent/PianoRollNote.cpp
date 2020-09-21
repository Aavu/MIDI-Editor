/*
  ==============================================================================

    PianoRollNote.cpp
    Created: 23 Mar 2020 2:05:16am
    Author:  Jiawen Huang

  ==============================================================================
*/

#include "PianoRollNote.h"

#include <utility>

PianoRollNote::PianoRollNote(std::shared_ptr<PlayerComponent> player, int row_n, float offset_n, float length_n /*= 1*/, int velocity_n /*= 120*/, MidiMessageSequence::MidiEventHolder* pNoteOnEvent /*= nullptr*/, MidiMessageSequence::MidiEventHolder* pNoteOffEvent /*= nullptr*/, NoteMessage *noteMessage_n /*= nullptr*/):
    m_pPlayer(std::move(player)),
    m_pNoteOnEvent(pNoteOnEvent),
    m_pNoteOffEvent(pNoteOffEvent),
    m_iRow(row_n),
    m_fOffset(offset_n),
    m_fLength(length_n),
    m_iVelocity(velocity_n),
    m_pNoteMessage(noteMessage_n),
    m_pBorder(nullptr)
{
    m_bInit = true; //TODO: This is not really needed if it'll be set to true when object is constructed
    m_iBoxWidth = Globals::PianoRoll::initNoteWidth;
    m_iBoxHeight = Globals::PianoRoll::initNoteHeight;

    m_pBorder = new PianoRollBorderComponent(this, NULL);
    addChildComponent(m_pBorder);
    m_pBorder->setBorderThickness (BorderSize<int> (0,1,0,1)); // allow draging from both sides
    m_pBorder->setBounds(this->getBounds());
    m_pBorder->setAlpha(0.1);
    m_pBorder->setVisible(true);
    
    m_pConstrainer = new ComponentBoundsConstrainer();
    m_pConstrainer->setMaximumHeight(m_iBoxHeight);
}

PianoRollNote::~PianoRollNote()
{
    delete m_pNoteMessage;
    delete m_pBorder;
    delete m_pConstrainer;
}

void PianoRollNote::resized()
{
    if (m_pBorder)
    {
        m_pBorder->setBounds(0,0,getWidth(),getHeight());
        // m_fLength = 1.F*getWidth() / m_iBoxWidth;
        auto newBounds = getBoundsInParent();
        m_fOffset = 1.F * newBounds.getX() / m_iBoxWidth;
        m_fLength = 1.F * newBounds.getWidth() / m_iBoxWidth;
        repaint();
    }
}

void PianoRollNote::mouseDown (const MouseEvent& event)
{
    // add to selectedNoteList
    getParentComponent()->mouseDown(event);
    m_pMyDragger.startDraggingComponent (this, event);

    repaint();
}

void PianoRollNote::mouseUp (const MouseEvent& event)
{
    auto * pSequence = m_pPlayer->getMidiMessageSequence();
    m_pPlayer->updateNote(m_pNoteOnEvent, m_pNoteOffEvent, m_fOffset, m_fLength, getNoteNumber());
//    DBG( "GUI Note On: " << m_pNoteOnEvent->message.getDescription() << " " << m_pNoteOnEvent->message.getTimeStamp());
//    DBG( "GUI Note Off: " << m_pNoteOffEvent->message.getDescription() << " " << m_pNoteOffEvent->message.getTimeStamp());
//    DBG( "GUI Note Length: " << m_pNoteOffEvent->message.getTimeStamp() - m_pNoteOnEvent->message.getTimeStamp());
    DBG("----------------");
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
        changePitch(this, -1);
        m_iRow--;
    }
    else if (event.getPosition().getY() > m_iBoxHeight && m_iRow < Globals::PianoRoll::midiNoteNum-1)
    {
        changePitch(this, 1);
        m_iRow++;
    }
    else
    {
        //std::cout << "stay" << std::endl;
    }

    repaint();
}

void PianoRollNote::mouseEnter(const MouseEvent& event)
{
    // std::cout << "mouse enter: " << m_iRow << std::endl;
    repaint();
    hightlightRow();
}

int PianoRollNote::getRow() { return m_iRow; }

float PianoRollNote::getOffset() { return m_fOffset; }
float PianoRollNote::getLength() { return m_fLength; }

NoteMessage* PianoRollNote::getNoteMessage() { return m_pNoteMessage; }

bool PianoRollNote::ifInit() { return m_bInit; }

int PianoRollNote::getNoteNumber() const { return Globals::PianoRoll::midiNoteNum-m_iRow+1; }

void PianoRollNote::setNoteOnEventPtr(MidiMessageSequence::MidiEventHolder * pNoteOnEvent) {
    m_pNoteOnEvent = pNoteOnEvent;
}

void PianoRollNote::setNoteOffEventPtr(MidiMessageSequence::MidiEventHolder * pNoteOffEvent) {
    m_pNoteOffEvent = pNoteOffEvent;
}

void PianoRollNote::deleteFromPlayer() {
    m_pPlayer->deleteNote(m_pPlayer->getMidiMessageSequence()->getIndexOf(m_pNoteOnEvent));
}

void PianoRollNote::addNoteToPlayer() {
    m_pPlayer->addNote(this);
}

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
    m_pNoteList[row].add(pianoRollNote);
}

void NoteList::detachNote(int row, PianoRollNote* noteToMove)
{
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

void NoteList::deleteNote(PianoRollNote *noteToBeRemoved, bool removeFromPlayer /*= false*/)
{
    if (removeFromPlayer)
        noteToBeRemoved->deleteFromPlayer();
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
        m_pNoteList->deleteNote(noteToBeRemoved, true);
    }
}
