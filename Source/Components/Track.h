/*
  ==============================================================================

    Track.h
    Created: 4 Mar 2020 1:32:40am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Track: public juce::Component {
protected:
    int m_iIndex = 0;
    bool m_bArmed;
    bool m_bEnabled;
    bool m_bMuted;

public:
    explicit Track(int id) :    m_bArmed(false),
                                m_bEnabled(true),
                                m_bMuted(false)
    {
        m_iIndex = id;
    }




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Track)
};
