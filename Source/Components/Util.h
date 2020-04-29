//
// Created by Raghavasimhan Sankaranarayanan on 4/28/20.
//
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Globals.h"

class CUtil {
public:
    static String getAbsolutePathOfProject(const String &projectFolderName = "MIDI-Editor") {
        File currentDir = File::getCurrentWorkingDirectory();

        while (currentDir.getFileName() != projectFolderName) {
            currentDir = currentDir.getParentDirectory();
            if (currentDir.getFullPathName() == "/")
                return String();
        }
        return currentDir.getFullPathName();
    }
};
