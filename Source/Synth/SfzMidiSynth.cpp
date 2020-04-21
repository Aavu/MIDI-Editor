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

void SfzSynth::handleProgramChange(int iMidiChannel, int iProgram) {
    std::cout<< "SfzSynth::handleProgramChange--> midiChannel: " << iMidiChannel << " programNumber: " << iProgram << std::endl;
    for (int s=0; s<getNumSounds(); s++) {
        auto *sound = getSound(iMidiChannel);
        if (sound->appliesToChannel(iMidiChannel)) {
            sound->useSubsound(iProgram);
            return;
        }
    }
}

int SfzSynth::getProgramNumber(int iMidiChannel) const {
    return getSound(iMidiChannel)->selectedSubsound();
}

juce::String SfzSynth::getProgramName(int iProgram) const {
    return getSound(0)->subsoundName(iProgram);
}

sfzero::Sound * SfzSynth::getSound(int iMidiChannel) const {
    return dynamic_cast<sfzero::Sound *>(sfzero::Synth::getSound(iMidiChannel).get());
}

void SfzSynth::addSound(sfzero::Sound *pSound) {
    sfzero::Synth::addSound(pSound);
}

//============================================================================================================
SfzLoader::SfzLoader() :
        m_loadThread(this),
        m_pSound(nullptr),
        m_fLoadProgress(0.0),
        m_callback(nullptr)
{
}

void SfzLoader::setSfzFile(File *pNewSfzFile)
{
    m_sfzFile = *pNewSfzFile;
}

void SfzLoader::loadSound(bool bUseLoaderThread /*= false*/, std::function<void()> *callback /*= nullptr*/)
{
    m_callback = *callback; //TODO: is this a good practice??
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

    m_pSound->useSubsound(0); // TODO: Use global variable to set default subsound?

    if (m_callback) {
        m_callback();
    }

    if (thread && thread->threadShouldExit())
    {
        return;
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

sfzero::Sound * SfzLoader::getLoadedSound() const {
    return m_pSound;
}