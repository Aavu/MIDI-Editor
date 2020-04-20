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
        static constexpr int iUpdateInterval_ms = 20;
        static constexpr int iPlayHeadWidth     = 4;
    }
}
#endif
