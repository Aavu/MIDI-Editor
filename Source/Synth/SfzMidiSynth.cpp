/*
  ==============================================================================

    SfzMidiSynth.cpp
    Created: 13 Mar 2020 7:08:24pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#include "SfzMidiSynth.h"

SfzSynthAudioSource::SfzSynthAudioSource(MidiKeyboardState &keyState) :
        keyboardState(keyState),
        loadProgress(0.0),
        loadThread(this)

{
    for (auto i=0; i<4; ++i)
        synth.addVoice(new sfzero::Voice());
}

void SfzSynthAudioSource::setUsingSineWaveSound() {
    synth.clearSounds();  //TODO: This seems incomplete. Shouldnt you do synth.addSound(new SineWaveSound()) here ??
}

void SfzSynthAudioSource::prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SfzSynthAudioSource::releaseResources() {}

void SfzSynthAudioSource::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();
    MidiBuffer incomingMidi;
    keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
}

void SfzSynthAudioSource::setSfzFile(File *newSfzFile)
{
    sfzFile = *newSfzFile;
    loadSound();
}

void SfzSynthAudioSource::setSfzFileThreaded(File *newSfzFile)
{
    loadThread.stopThread(2000);
    sfzFile = *newSfzFile;
    loadThread.startThread();
}


void SfzSynthAudioSource::loadSound(Thread *thread)
{
    loadProgress = 0.0;
    synth.clearSounds();

    if (!sfzFile.existsAsFile())
    {
        return;
    }

    sfzero::Sound *sound;
    auto extension = sfzFile.getFileExtension();
    if ((extension == ".sf2") || (extension == ".SF2"))
    {
        sound = new sfzero::SF2Sound(sfzFile);
    }
    else
    {
        sound = new sfzero::Sound(sfzFile);
    }
    sound->loadRegions();
    sound->loadSamples(formatManager, &loadProgress, thread);

    std::cout<< "Load Progress: " << loadProgress << std::endl;

    if (thread && thread->threadShouldExit())
    {
        delete sound;
        return;
    }

    synth.addSound(sound);
    std::cout << sound->numSubsounds() << std::endl;
    sound->useSubsound(71);
}

SfzSynthAudioSource::LoadThread::LoadThread(SfzSynthAudioSource *sfzSynthAudioSrc)
        : Thread("SFZLoad"), sfzSynthAudioSource(sfzSynthAudioSrc)
{
}

void SfzSynthAudioSource::LoadThread::run()
{
    sfzSynthAudioSource->loadSound(this);
}
