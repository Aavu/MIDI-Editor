/*
  ==============================================================================

    PianoRollListComponent.h
    Created: 27 Apr 2020 7:41:15pm
    Author:  Jiawen Huang
 
    A GUI component which takes care of multiple tracks, the playhead, and
 switch between edit mode and preview mode.
    The multi-track functionality is not implemented yet. Currently only works
 for one track.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "../TrackParameters.h"
#include "../PlayHeadComponent.h"
#include "../PlayHeadScrollComponent.h"
#include "../PlayerComponent.h"
#include "../Globals.h"

#include "ScrollablePianoRollComponent/ScrollablePianoRollComponent.h"
#include "ScrollablePianoRollComponent/PianoRollNote.h"

class PianoRollListComponent: public TrackParameters, public Component, public Timer {
public:
    PianoRollListComponent();
    
    ~PianoRollListComponent() override;
    
    void init(std::shared_ptr<PlayerComponent> player);
    void paint(Graphics& g) override;
    void resized() override;
    
    int getNumTracks() const;
    
    void addTrack(int numTimeStampsForPianoRoll);
    
    void setTimeFormat(int timeFormat);
    
    void convertMidiMessageSequence(int trackIdx, const MidiMessageSequence *message);
    
private:
    void timerCallback() override;
    void updatePlayHeadPosition();
    void handleScrollCallback(int newPositionX);
    
    std::vector<ScrollablePianoRollComponent*> m_tracks;
    
    int m_iTimeFormat;
    
    std::shared_ptr<PlayerComponent> m_pPlayer;
    PlayHeadScrollComponent m_playHeadScroll;
    std::shared_ptr<PlayHeadComponent> m_pPlayHead = nullptr;
    
    long m_iMaxBufferLength = 0;
    long m_iCurrentPlayHeadPosition = 0;
    int m_iPianoRollListComponentWidth = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRollListComponent);
};
