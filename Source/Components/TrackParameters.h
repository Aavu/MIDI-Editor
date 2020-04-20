/*
  ==============================================================================

    TrackParameters.h
    Created: 4 Mar 2020 1:19:51am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


class TrackParameters {
public:
    static int m_iNumTracks;
    static int k_iDefaultTrackHeight;
    static std::vector<int> m_aiTrackHeight;

    TrackParameters() = default;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackParameters)
};