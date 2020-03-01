# MIDI-Editor

## problems: paint note component
* a note should be a button laying above the grid (addAndMakeVisible)
* things cannot be painted out of the bounds, so that painting within the paint function of a cell is not gonna work
* painting within the row paint function is a good idea, however RowComp is a private class in TableListBox. I cannot change the paint behavior.

## things to try
* under my pianoroll table, paint the notes after painting the table (make it scroll with the grid)
* create another ListBox to hold all the notes (the PianoRollTableListBox only provides the grid and position suggestions)
	* click on the grid -> add to notelist
	* call the ListBox to update
