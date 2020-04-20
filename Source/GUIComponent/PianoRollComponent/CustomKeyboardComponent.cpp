/*
  ==============================================================================

    CustomKeyBoard.cpp
    Created: 23 Mar 2020 3:30:20am
    Author:  Jiawen Huang

  ==============================================================================
*/

#include "CustomKeyboardComponent.h"
#include "../../Globals.h"

CustomKeyboardComponent::CustomKeyboardComponent(): MidiKeyboardComponent(*this, MidiKeyboardComponent::verticalKeyboardFacingRight)
{
    setSize(80, Globals::initTimeStamps*Globals::initNoteWidth);
    setBlackNoteWidthProportion(m_fRatio);
    setBlackNoteLengthProportion(0.8f);
    setKeyWidth(Globals::initNoteHeight*12.0/7);
    setScrollButtonsVisible(false);
}

Range<float> CustomKeyboardComponent::getKeyPosition(int midiNoteNumber,float keyWidth) const
{
    jassert (midiNoteNumber >= 0 && midiNoteNumber < 128);
    
    static const float notePos[] = { 0.0f, static_cast<float>(7.0/12*1.5 - m_fRatio * 0.5f),
        7.0/12*1.5, static_cast<float>(7.0/12*3.5 - m_fRatio * 0.5f),
        7.0/12*3.5,
        7.0/12*5, static_cast<float>(7.0/12*6.5 - m_fRatio * 0.5f),
        7.0/12*6.5, static_cast<float>(7.0/12*8.5 - m_fRatio * 0.5f),
        7.0/12*8.5, static_cast<float>(7.0/12*10.5 - m_fRatio * 0.5f),
        7.0/12*10.5, 7.0f };
    
    auto octave = midiNoteNumber / 12;
    auto note   = midiNoteNumber % 12;
    
    auto start = octave * 7.0f * keyWidth + notePos[note] * keyWidth;
    auto width = MidiMessage::isMidiNoteBlack (note) ? m_fRatio * keyWidth : (notePos[note+1]-notePos[note])*keyWidth;
    
    return { start, start + width };
}

String CustomKeyboardComponent::getWhiteNoteText (int midiNoteNumber)
{
    if (midiNoteNumber % 12 == 0 && !m_bPreview)
        return MidiMessage::getMidiNoteName (midiNoteNumber, true, true, 3);
    
    return {};
}

void CustomKeyboardComponent::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    Component::mouseWheelMove(e, wheel);
}

void CustomKeyboardComponent::setPreview(bool ifPreview)
{
    m_bPreview = ifPreview;
}
