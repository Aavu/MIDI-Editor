/*
  ==============================================================================

    TrackViewComponent.h
    Created: 3 Mar 2020 8:32:54pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/
#pragma once

#include <JuceHeader.h>
#include "SidebarComponent.h"
#include "PlayHeadComponent.h"
#include "PlayHeadScrollComponent.h"
#include "PlayerComponent.h"
#include "Globals.h"

#include "GUIComponent/ScrollablePianoRollComponent/ScrollablePianoRollComponent.h"
#include "GUIComponent/ScrollablePianoRollComponent/PianoRollNote.h"
#include "GUIComponent/PianoRollListComponent.h"

class TrackViewComponent : public Component {
public:
    TrackViewComponent();

    ~TrackViewComponent() override;

    void init(PlayerComponent* player);
    void paint(Graphics& g) override;
    void resized() override;

    int getNumTracks();
    
    void addTrack(int numTimeStampsForPianoRoll);
    
    void setTimeFormat(int timeFormat);
    
    void convertMidiMessageSequence(int trackIdx, const MidiMessageSequence *message);

private:
    
    PianoRollListComponent *m_pianoRollListComp;
    
    long m_iMaxBufferLength = 0;
    long m_iCurrentPlayHeadPosition = 0;
    int m_iTrackViewComponentWidth = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackViewComponent)
};
