/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    addAndMakeVisible(transportBar);
    addAndMakeVisible(menu);
    menu.setCallback(std::bind(&MainComponent::fileCallback, this, std::placeholders::_1));

    setSize (1000, 500);

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
    }

    // Create Player
    player = new PlayerComponent();
    transportBar.init(player);

    //TracksView
    trackView.init(0);
    addAndMakeVisible(trackView);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    delete player;
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
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
//    g.setFont (Font (16.0f));
//    g.setColour (Colours::white);
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    Rectangle<int> bounds = getLocalBounds();
    int height = bounds.getHeight();
    bounds.setHeight(64);
    bounds.setY(height - 64);
    transportBar.setBounds(bounds);

    bounds = getLocalBounds();
    bounds.removeFromBottom(transportBar.getBounds().getHeight());
    trackView.setBounds(bounds);
}

bool MainComponent::fileCallback(CommandID commandID) {
    switch (commandID)
    {
        case Menu::fileOpen:
            handleFileOpen();
            break;

        case Menu::fileExportAudio:
            break;

        case Menu::fileExportMIDI:
            break;

        default:
            return false;
    }
    return true;
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
        trackView.addTrack();
        // Add sequence to player

        player->setMidiMessageSequence(sequence);
        delete stream;
    }
}