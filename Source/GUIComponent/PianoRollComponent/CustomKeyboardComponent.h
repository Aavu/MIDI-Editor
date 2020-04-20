/*
  ==============================================================================

    CustomKeyBoard.h
    Created: 23 Mar 2020 3:30:20am
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Globals.h"

class CustomKeyboardComponent: public MidiKeyboardState, public MidiKeyboardComponent
{
public:
    CustomKeyboardComponent();
    
    Range<float> getKeyPosition (int midiNoteNumber,float keyWidth) const override;
    
    String getWhiteNoteText (int midiNoteNumber) override;

    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel) override;
    
    void setPreview(bool ifPreview);

private:
    float           m_fRatio = 0.7;
    bool            m_bPreview = true;
};
