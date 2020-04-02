/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent() :
        synthAudioSource(keyboardState),
        keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure you set the size of the component after
    // you add any child components.
    addAndMakeVisible(transportBar);
    addAndMakeVisible(menu);
    menu.setCallback(std::bind(&MainComponent::fileCallback, this, std::placeholders::_1));

    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
        addAndMakeVisible(keyboardComponent);
        setSize(600, 160);
        startTimer(400);

        synthAudioSource.setSfzFile(new File(getAbsolutePathOfProject() + "/Resources/SoundFonts/GeneralUser GS 1.442 MuseScore/GeneralUser GS MuseScore v1.442.sf2"));

    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    synthAudioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    synthAudioSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    synthAudioSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setFont (Font (16.0f));
    g.setColour (Colours::white);
    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    Rectangle<int> transportBounds = getLocalBounds();
    int height = transportBounds.getHeight();
    transportBounds.setHeight(64);
    transportBounds.setY(height - 64);
    transportBar.setBounds(transportBounds);
}

bool MainComponent::fileCallback(CommandID commandID) {
    switch (commandID)
    {
        case MenuComponent::fileOpen:
            handleFileOpen();
            break;

        case MenuComponent::fileExportAudio:
            break;

        case MenuComponent::fileExportMIDI:
            break;

        default:
            return false;
    }
    return true;
    keyboardComponent.setBounds(10,10, getWidth()-20, getHeight()-20);
}

void MainComponent::timerCallback() {
    keyboardComponent.grabKeyboardFocus();
    stopTimer();
}

String MainComponent::getAbsolutePathOfProject(const String &projectFolderName) {
    File currentDir = File::getCurrentWorkingDirectory();

    while (currentDir.getFileName() != projectFolderName) {
        currentDir = currentDir.getParentDirectory();
        if (currentDir.getFullPathName() == "/")
           return String();
    }
    return currentDir.getFullPathName();
}

void MainComponent::handleFileOpen() {
    FileChooser chooser ("Select a MIDI file to play...", {}, "*.mid");

    if (chooser.browseForFileToOpen()) {
        auto file = chooser.getResult();
        FileInputStream* stream = file.createInputStream();
        bool err = midiFile.readFrom(*stream);
        if (!err) {
            std::cerr << "Error readFrom MidiFile" << std::endl;
            return;
        }
        const MidiMessageSequence* sequence = midiFile.getTrack(0);
        midiFile.convertTimestampTicksToSeconds();
        auto lastTime = midiFile.getLastTimestamp();
        std::cout << lastTime << std::endl;
        int i = 0;
        double time = 0;
        while(time < lastTime) {
            juce::MidiMessageSequence::MidiEventHolder* holder = sequence->getEventPointer(i);
            MidiMessage msg = holder->message;
            int noteNumber = msg.getNoteNumber();
            String note = msg.getMidiNoteName(noteNumber, true, true, 3);
            time = msg.getTimeStamp();
            std::cout << note << "\t" << time << "\t" << msg.getFloatVelocity() << std::endl;
            i++;
        }
        delete stream;
    }
}