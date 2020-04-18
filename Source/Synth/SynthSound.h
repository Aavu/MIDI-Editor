//
// Created by Raghavasimhan Sankaranarayanan on 4/17/20.
//

#pragma once

#include "../../JuceLibraryCode/JuceHeader.h"


class SynthSound : public SynthesiserSound {
public:
    bool appliesToNote (int midiNoteNumber) override {
        return true;
    }

    bool appliesToChannel (int midiChannel) override {
        return true;
    }
};
