/*
  ==============================================================================

    SfzMidiSynth.cpp
    Created: 13 Mar 2020 7:08:24pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#include "SfzMidiSynth.h"

SfzSynthAudioSource::SfzSynthAudioSource(MidiKeyboardState &keyState) :
        m_keyboardState(keyState)
{
    for (auto i=0; i<4; ++i)
        m_synth.addVoice(new sfzero::Voice());
}

void SfzSynthAudioSource::setUsingSineWaveSound() {
    m_synth.clearSounds();  //TODO: This seems incomplete. Shouldnt you do m_synth.addSound(new SineWaveSound()) here ??
}

void SfzSynthAudioSource::prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) {
    m_synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SfzSynthAudioSource::releaseResources() {}

void SfzSynthAudioSource::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    m_keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
    m_synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
}

int SfzSynthAudioSource::getProgramNumber() const {
    return getSound()->selectedSubsound();
}

juce::String SfzSynthAudioSource::getProgramName() const {
    sfzero::Sound *sound = getSound();
    return sound->subsoundName(sound->selectedSubsound());
}

void SfzSynthAudioSource::setProgramNumber(int iProgramNum) {
    getSound()->useSubsound(iProgramNum);
}

sfzero::Sound * SfzSynthAudioSource::getSound() const {
    SynthesiserSound * s = m_synth.getSound(0).get();
    return dynamic_cast<sfzero::Sound *>(s);
}

void SfzSynthAudioSource::addSound(sfzero::Sound *sound) {
    m_synth.clearSounds();
    m_synth.addSound(sound);
}

//============================================================================================================
SfzLoader::SfzLoader() :
        m_loadThread(this),
        m_fLoadProgress(0.0),
        m_pSound(nullptr)
{
}

void SfzLoader::setSfzFile(File *pNewSfzFile)
{
    m_sfzFile = *pNewSfzFile;
}

void SfzLoader::loadSound(bool bUseLoaderThread)
{
    if (bUseLoaderThread) { // Uses separate thread
        m_loadThread.stopThread(2000);
        m_loadThread.startThread();
    }
    else {
        load();
    }
}

double SfzLoader::getLoadProgress() const {
    return m_fLoadProgress;
}

void SfzLoader::load(Thread *thread)
{
    m_fLoadProgress = 0.0;

    if (!m_sfzFile.existsAsFile())
    {
        std::cout << "Invalid Soundfont File." << std::endl; //TODO: handle errors in a better way
        return;
    }

    auto extension = m_sfzFile.getFileExtension();
    if ((extension == ".sf2") || (extension == ".SF2"))
    {
        m_pSound = new sfzero::SF2Sound(m_sfzFile);
    }
    else
    {
        m_pSound = new sfzero::Sound(m_sfzFile);
    }
    m_pSound->loadRegions();
    m_pSound->loadSamples(m_formatManager, &m_fLoadProgress, thread);

    std::cout<< "Load Progress: " << m_fLoadProgress << std::endl;

    if (thread && thread->threadShouldExit())
    {
        return;
    }

    m_pSound->useSubsound(0); // TODO: Use global variable to set default subsound?
}

SfzLoader::LoadThread::LoadThread(SfzLoader *pSfzLoader) :
        Thread("SFZLoad"),
        m_pSfzLoader(pSfzLoader)
{
}

void SfzLoader::LoadThread::run()
{
    m_pSfzLoader->load(this);
}

sfzero::Sound * SfzLoader::getLoadedSound() const {
    return m_pSound;
}