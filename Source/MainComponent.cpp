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
    addAndMakeVisible(m_transportBar);
    addAndMakeVisible(m_menu);
    m_menu.setCallback(std::bind(&MainComponent::fileCallback, this, std::placeholders::_1));

    // Create Player
    // Create Player
    m_pPlayer = new PlayerComponent();
    m_transportBar.init(m_pPlayer);

    //TracksView
    m_trackView.init(0);
    addAndMakeVisible(m_trackView);

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

    if (auto* device = deviceManager.getCurrentAudioDevice())
    {
        m_fSampleRate = device->getCurrentSampleRate();
        m_iBitDepth = device->getCurrentBitDepth();
        DBG ("Current audio device: "   + device->getName().quoted());
        DBG ("Sample rate: "    + String (m_fSampleRate) + " Hz");
        DBG ("Block size: "     + String (device->getCurrentBufferSizeSamples()) + " samples");
        DBG ("Bit depth: "      + String (m_iBitDepth));
    }
    else
    {
        DBG ("No audio device open");
    }

    addActionListener(m_audioExporter);

    setSize (1000, 500);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    delete m_pPlayer;
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
    m_pPlayer->prepareToPlay(samplesPerBlockExpected, sampleRate);
    m_fSampleRate = sampleRate;
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    m_iNumChannels = bufferToFill.buffer->getNumChannels();
    m_pPlayer->getNextAudioBlock(bufferToFill);
    if (m_audioExporter)
        m_audioExporter->write(bufferToFill);
//    AudioBuffer<float>* buffer = bufferToFill.buffer;
//    for (int i=0; i<bufferToFill.numSamples; i++) {
//        std::cout << buffer->getSample(0, i) << " ";
//    }
//    std::cout << std::endl;

}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    removeAllActionListeners();
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
    m_transportBar.setBounds(bounds);

    bounds = getLocalBounds();
    bounds.removeFromBottom(m_transportBar.getBounds().getHeight());
    m_trackView.setBounds(bounds);
}

bool MainComponent::fileCallback(CommandID commandID) {
    switch (commandID)
    {
        case MenuComponent::fileOpen:
            handleFileOpen();
            break;

        case MenuComponent::fileExportAudio:
            handleExportAudio();
            break;

        case MenuComponent::fileExportMIDI:
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
        bool err = m_midiFile.readFrom(*stream);
        if (!err) {
            std::cerr << "Error readFrom MidiFile" << std::endl;
            return;
        }
        const MidiMessageSequence* sequence = m_midiFile.getTrack(0);
        m_midiFile.convertTimestampTicksToSeconds();
        m_trackView.addTrack();

        m_pPlayer->setMidiMessageSequence(sequence);
        delete stream;
    }
}

void MainComponent::handleExportAudio() {
    FileChooser chooser("Select path to save WAV file...");

    if (chooser.browseForFileToSave(true)) {
        auto file = chooser.getResult();
        m_audioExporter = new AudioExportComponent(m_fSampleRate, m_iNumChannels, 16); // hard coded bitdepth for now
        m_audioExporter->init(file);
    }
}

void MainComponent::actionListenerCallback (const String& message) {
    if (message == "stop") {
        sendActionMessage("finishExport");
    }
}