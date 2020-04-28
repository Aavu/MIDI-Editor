/*
  ==============================================================================

    Globals.h
    Created: 12 Jan 2020 12:27:52pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#ifndef GLOBALS
#define GLOBALS

namespace Globals {
    namespace ActionMessage {
        static const String PlayForExport   = "AM_PlayForExport";
        static const String Stop            = "AM_Stop";
    }

    namespace GUI {
        static constexpr float fFramesPerSecond = 30;
        static constexpr int iUpdateInterval_ms = 1000/fFramesPerSecond;
        static constexpr int iPlayHeadWidth     = 2;
        static constexpr int iSideBarWidth      = 128;
        static constexpr int iHeaderHeight      = 32;
    }
    
    namespace PianoRoll {
        // piano roll parameters
        const int midiNoteNum       = 128;
        const int initTimeStamps    = 30;
        const int initNoteHeight    = 8;
        const int initNoteWidth     = 40;
        const int longestNote       = 20;
        const int keyboardWidth     = 40;
    }

    namespace Transport {
        static double fBpm = 120;
    }

}
#endif
