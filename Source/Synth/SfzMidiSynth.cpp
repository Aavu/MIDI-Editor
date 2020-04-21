/*
  ==============================================================================

    SfzMidiSynth.cpp
    Created: 13 Mar 2020 7:08:24pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#include "SfzMidiSynth.h"

SfzSynth::SfzSynth()
{
    for (auto i=0; i<4; ++i)
        addVoice(new sfzero::Voice());
}

void SfzSynth::setUsingSineWaveSound() {
    clearSounds();  //TODO: This seems incomplete. Shouldnt you do m_synth.addSound(new SineWaveSound()) here ??
}

int SfzSynth::getProgramNumber() const {
    return getSound()->selectedSubsound();
}

juce::String SfzSynth::getProgramName() const {
    sfzero::Sound *sound = getSound();
    return sound->subsoundName(sound->selectedSubsound());
}

void SfzSynth::setProgramNumber(int iProgramNum) {
    getSound()->useSubsound(iProgramNum);
}

sfzero::Sound * SfzSynth::getSound() const {
    SynthesiserSound * s = sfzero::Synth::getSound(0).get();
    return dynamic_cast<sfzero::Sound *>(s);
}

void SfzSynth::addSound(sfzero::Sound *sound) {
    sfzero::Synth::addSound(sound);
}

//============================================================================================================
SfzLoader::SfzLoader() :
        m_loadThread(this),
        m_fLoadProgress(0.0),
        m_iNumInstances(0),
        m_callback(nullptr)
{
}

void SfzLoader::setSfzFile(File *pNewSfzFile)
{
    m_sfzFile = *pNewSfzFile;
}

void SfzLoader::loadSounds(int iNumInstances /*= 1*/, bool bUseLoaderThread /*= false*/, std::function<void()> *callback /*= nullptr*/)
{
    m_callback = *callback;
    m_iNumInstances = iNumInstances;
    if (bUseLoaderThread) {
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
    if (!m_sfzFile.existsAsFile()) {
        std::cout << "Invalid Soundfont File." << std::endl; //TODO: handle errors in a better way
        return;
    }

    m_fLoadProgress = 0.0;
    auto extension = m_sfzFile.getFileExtension();
    sfzero::Sound * pSoundInstance;

    for (auto i=0; i<m_iNumInstances; i++) {
        if ((extension == ".sf2") || (extension == ".SF2")) {
            pSoundInstance = new sfzero::SF2Sound(m_sfzFile);
        } else {
            pSoundInstance = new sfzero::Sound(m_sfzFile);
        }
        pSoundInstance->loadRegions();
        pSoundInstance->loadSamples(m_formatManager, &m_fLoadProgress, thread);
        m_sounds.add(pSoundInstance);

        if (thread && thread->threadShouldExit()) {
            return;
        }
    }

    if (m_callback) {
        m_callback();
    }
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

ReferenceCountedArray<sfzero::Sound> SfzLoader::getLoadedSounds() const {
    return m_sounds;
}