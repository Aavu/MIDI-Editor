# MIDI Editor

MIDI Editor is an open-source project with the aim of building a simple lightweight MIDI file player and editor (in development). The player supports [General MIDI](https://en.wikipedia.org/wiki/General_MIDI) 1.0 spec. It uses the [GeneralUser GS MuseScore v1.442](http://schristiancollins.com/soundfonts/GeneralUser_GS_1.442-MuseScore.zip) soundfont file from [MuseScore](https://musescore.org/en/handbook/soundfonts-and-sfz-files#list) to generate sounds. You can check out the [Demo Video](https://www.youtube.com/watch?v=iFSt7ScsCPo) (Please Turn ON Captions for explanations).

## Build Notes:
Only Mac OSX is currently supported.

## Requirements
Juce

## Building the App
- Use the Projucer application to build the app from source.
- After building the binary, copy the Resources folder and paste it onto `<your build folder>/MidiEditor.app/Contents/`
- Double click on the MidiEditor.app to use it.

## Usage:

### How to use:

- **Open MIDI file:** Using the menubar go to:`File>Open` and then select a MIDI file. 
- **Playback:** Use the controls on the bottom left of the interface.

### Interface:
<img align="center" src="https://github.com/Aavu/MIDI-Editor/blob/master/Documentation/images/MidiEditorInterface.png" width=1024>



## Contributors:
- [Jiawen Huang](https://github.com/jhuang448)
- [Kaushal Sali](https://www.kaushalsali.com)
- [Raghavasimhan Sankaranarayanan](https://techitunes.com)
