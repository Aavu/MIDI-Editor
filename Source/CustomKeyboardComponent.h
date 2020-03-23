/*
  ==============================================================================

    CustomKeyBoard.h
    Created: 23 Mar 2020 3:30:20am
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class CustomKeyboardComponent: public MidiKeyboardComponent, public MidiKeyboardState
{
public:
    CustomKeyboardComponent();
    
    Range<float> getKeyPosition(int midiNoteNumber,float keyWidth) const override;
private:
    float           ratio=0.7;
};
