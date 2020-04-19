//
// Created by Raghavasimhan Sankaranarayanan on 4/18/20.
//

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class PlayHeadComponent : public AudioPlayHead {
public:
    PlayHeadComponent();

private:
    AudioPlayHead::CurrentPositionInfo m_currentPositionInfo;
};
