/*
  ==============================================================================

    SfzMidiSynth.cpp
    Created: 13 Mar 2020 7:08:24pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#include "SfzMidiSynth.h"

SfzSynthAudioSource::SfzSynthAudioSource(MidiKeyboardState &keyState) :
        m_keyboardState(keyState),
        m_fLoadProgress(0.0),
        m_loadThread(this)

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

void SfzSynthAudioSource::setSfzFile(File *newSfzFile)
{
    m_sfzFile = *newSfzFile;
    loadSound();
}

void SfzSynthAudioSource::setSfzFileThreaded(File *newSfzFile)
{
    m_loadThread.stopThread(2000);
    m_sfzFile = *newSfzFile;
    m_loadThread.startThread();
}


void SfzSynthAudioSource::loadSound(Thread *thread)
{
    m_fLoadProgress = 0.0;
    m_synth.clearSounds();

    if (!m_sfzFile.existsAsFile())
    {
        return;
    }

    sfzero::Sound *sound;
    auto extension = m_sfzFile.getFileExtension();
    if ((extension == ".sf2") || (extension == ".SF2"))
    {
        sound = new sfzero::SF2Sound(m_sfzFile);
    }
    else
    {
        sound = new sfzero::Sound(m_sfzFile);
    }
    sound->loadRegions();
    sound->loadSamples(m_formatManager, &m_fLoadProgress, thread);

    std::cout<< "Load Progress: " << m_fLoadProgress << std::endl;

    if (thread && thread->threadShouldExit())
    {
        delete sound;
        return;
    }

    m_synth.addSound(sound);
    std::cout << sound->numSubsounds() << std::endl;
    sound->useSubsound(13);
}

SfzSynthAudioSource::LoadThread::LoadThread(SfzSynthAudioSource *sfzSynthAudioSrc)
        : Thread("SFZLoad"), m_pSfzSynthAudioSource(sfzSynthAudioSrc)
{
}

void SfzSynthAudioSource::LoadThread::run()
{
    m_pSfzSynthAudioSource->loadSound(this);
}
