/*
  ==============================================================================

    PianoRollNote.cpp
    Created: 23 Mar 2020 2:05:16am
    Author:  Jiawen Huang

  ==============================================================================
*/

#include "PianoRollNote.h"

PianoRollNote::PianoRollNote(int row_n, float offset_n, float length_n, int velocity_n, NoteMessage *noteMessage_n):
    row(row_n),
    offset(offset_n), length(length_n),
    velocity(velocity_n), noteMessage(noteMessage_n),
    border(0)
{
    init = true;
    boxWidth = Globals::noteWidth;
    boxHeight = Globals::noteHeight;
    
    border = new ResizableBorderComponent(this, NULL);
    addChildComponent(border);
    border->setBorderThickness (BorderSize<int> (0,0,0,1));
    border->setBounds(this->getBounds());
    border->setAlpha(0.1);
    border->setVisible(true);
    
    constrainer = new ComponentBoundsConstrainer();
    constrainer->setMaximumHeight(boxHeight);
    std::cout << "create " << row << ' ' << offset_n << std::endl;
}

PianoRollNote::~PianoRollNote()
{
    if (noteMessage)
        delete noteMessage;
    if (border)
        delete border;
    if (constrainer)
        delete constrainer;
    std::cout << "delete " << row << ' ' << offset << std::endl;
}

void PianoRollNote::resized()
{
    // TODO: not working now
    if (border)
    {
        border->setBounds(0,0,getWidth(),getHeight());
        length = 1.F*getWidth() / boxWidth;
        std::cout << length << std::endl;
        repaint();
    }
}

void PianoRollNote::mouseDown (const MouseEvent& event)
{
    // add to selectedNoteList
    getParentComponent()->mouseDown(event);
    myDragger.startDraggingComponent (this, event);
}

void PianoRollNote::mouseDrag (const MouseEvent& event)
{
    myDragger.dragComponent (this, event, constrainer);
    if (constrainer)
    {
        auto newBounds = getBoundsInParent();
        //std::cout << offset * boxWidth << ' '<< newBounds.getX() << std::endl;
        setBounds (newBounds);
        offset = 1.F * newBounds.getX() / boxWidth;
        constrainer->setMinimumOnscreenAmounts (getHeight(), getWidth(), getHeight(), getWidth());
    }
    if (event.getPosition().getY() < 0 && row > 0)
    {
        std::cout << "move up" << std::endl;
        changePitch(this, -1);
        row--;
    }
    else if (event.getPosition().getY() > boxHeight && row < Globals::midiNoteNum-1)
    {
        std::cout << "move down" << std::endl;
        changePitch(this, 1);
        row++;
    }
    else
    {
        //std::cout << "stay" << std::endl;
    }
}

void PianoRollNote::mouseEnter(const MouseEvent& event)
{
    std::cout << "mouse enter: " << row << std::endl;
}

int PianoRollNote::getRow() { return row; }

float PianoRollNote::getOffset() { return offset; }
float PianoRollNote::getLength() { return length; }

NoteMessage* PianoRollNote::getNoteMessage() { return noteMessage; }

bool PianoRollNote::ifInit() { return init; }

void PianoRollNote::paintButton (Graphics &g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.setColour (Colours::limegreen);
    g.fillRoundedRectangle(0, 0, length*boxWidth, boxHeight, 4);
    g.setColour (Colours::lime);
    g.drawRoundedRectangle(0, 0, length*boxWidth, boxHeight, 4, 1);
}

//------------------NoteList---------------------
NoteList::NoteList(SelectedNoteList *selected_n)
{
    selected = selected_n;
    noteList = new Array<PianoRollNote*> [Globals::midiNoteNum];
    for (int i = 0; i < Globals::midiNoteNum; i++) {
        noteList[i] = Array<PianoRollNote*>();
    }
}

NoteList::~NoteList()
{
    for (int i = 0; i < Globals::midiNoteNum; i++) {
        for (int j = noteList[i].size() - 1; j >= 0 ; j--)
            if (getNote(i, j))
                deleteNote(i, j);
        noteList[i].clear();
    }
    delete [] noteList;
}

PianoRollNote* NoteList::getNote(int row, int idx)
{
    return noteList[row][idx];
}

Array<PianoRollNote*> NoteList::getNotesByRow(int row)
{
    return noteList[row];
}

void NoteList::addNote(int row, PianoRollNote* pianoRollNote)
{
    std::cout << "add Note to row: " << row << " offset: " << pianoRollNote->getOffset() << std::endl;

    noteList[row].add(pianoRollNote);
}

void NoteList::detachNote(int row, PianoRollNote* noteToMove)
{
    std::cout << "remove Note from row: " << row << " offset: " << noteToMove->getOffset() << std::endl;
    noteList[row].removeFirstMatchingValue(noteToMove);
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
        noteList[row].remove(idx);
        delete noteToBeRemoved;
    }
}

void NoteList::deleteNote(PianoRollNote *noteToBeRemoved)
{
    int row = noteToBeRemoved->getRow();
    noteList[row].removeFirstMatchingValue(noteToBeRemoved);
    delete noteToBeRemoved;
}


//----------------SelectedNoteList--------------------

SelectedNoteList::SelectedNoteList(NoteList* noteList_n)
{
    selected = new Array<PianoRollNote*> ();
    noteList = noteList_n;
}
SelectedNoteList::~SelectedNoteList()
{
    selected->clear();
    delete selected;
}
void SelectedNoteList::selectOneNote(PianoRollNote* newNote)
{
    selected->clear();
    selected->add(newNote);
}

void SelectedNoteList::addSelectedNotes(PianoRollNote* newNote)
{
    selected->add(newNote);
}

void SelectedNoteList::removeSelectedNotes()
{
    PianoRollNote* noteToBeRemoved = 0;
    while (!selected->isEmpty())
    {
        noteToBeRemoved = selected->removeAndReturn(0);
        noteList->deleteNote(noteToBeRemoved);
    }
}
