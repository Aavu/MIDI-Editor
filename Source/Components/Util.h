//
// Created by Raghavasimhan Sankaranarayanan on 4/28/20.
//
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Globals.h"

class CUtil {
public:
    static String getResourcePath() {
        File app = File::getSpecialLocation(File::currentApplicationFile);
        auto resourcesDir = app.getChildFile("Contents").getChildFile("Resources");
        return resourcesDir.getFullPathName();
    }
};
