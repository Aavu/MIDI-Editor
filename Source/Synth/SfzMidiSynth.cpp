/*
  ==============================================================================

    SfzMidiSynth.cpp
    Created: 13 Mar 2020 7:08:24pm
    Author:  Kaushal Sali

  ==============================================================================
*/

#include "SfzMidiSynth.h"

SfzSynth::SfzSynth() :
    m_sfzLoader(new SfzLoader())
{
}

void SfzSynth::handleProgramChange(int iMidiChannel, int iProgram) {
    auto *sound = getSoundForChannel(iMidiChannel);
    if (sound) {
        sound->useSubsound(iProgram);
        DBG("SfzSynth::handleProgramChange-->  midiChannel: " << iMidiChannel << " set to programNumber: " << iProgram);
    }
}

void SfzSynth::initSynth(File * pSoundFontFile) {
    clearVoices();
    for (int i=0; i < kiNumVoices; i++) {
        addVoice(new sfzero::Voice());
    }
    // Load and add sounds
    // Create callback that will assign a midi channel to each sound and set sound of channel 10 to percussion.
    std::function<void()> addLoadedSoundCallback = [this] () {
        auto sounds = m_sfzLoader->getLoadedSounds();
        for (auto i=0; i<sounds.size(); i++) {
            auto * sound = sounds.getUnchecked(i).get();
            sound->setChannelNum(i);
            if (i == kiPercussionChannelNum)
                sound->useSubsound(kiPercussionSubSoundNum);
            addSound(sound);
        }
        DBG( sounds.size() << " sounds added.");
    };
    m_sfzLoader->setSfzFile(pSoundFontFile);
    DBG( "Loading sounds...");
    m_sfzLoader->loadSounds(kiNumChannels, true, &addLoadedSoundCallback);
}

int SfzSynth::getProgramNumber(int iMidiChannel) const {
    sfzero::Sound * sound = getSoundForChannel(iMidiChannel);
    if (sound)
        return sound->selectedSubsound();
    return -1;
}

juce::String SfzSynth::getProgramName(int iProgram) const {
    sfzero::Sound * sound = getSoundForChannel(0);
    if (sound)
        return sound->subsoundName(iProgram);
    return juce::String();
}

void SfzSynth::setProgramNumber(int iProgramNum, int iMidiChannel) {
    sfzero::Sound * sound = getSoundForChannel(iMidiChannel);
    if (sound)
        sound->useSubsound(iProgramNum);
}

void SfzSynth::resetProgramSelection() {
    for (auto i=0; i<getNumSounds(); i++) {
        if (i == kiPercussionChannelNum)
            getSoundForChannel(i)->useSubsound(kiPercussionSubSoundNum);
        else
            getSoundForChannel(i)->useSubsound(0);
    }
}

sfzero::Sound * SfzSynth::getSoundForChannel(int iMidiChannel) const {
    for (int i=0; i<getNumSounds(); i++) {
        auto *sound = dynamic_cast<sfzero::Sound *>(getSound(i).get());
        if (sound->appliesToChannel(iMidiChannel)) {
            return sound;
        }
    }
    return nullptr;
}

void SfzSynth::addSound(sfzero::Sound *pSound) {
    sfzero::Synth::addSound(pSound);
}

//============================================================================================================
SfzLoader::SfzLoader() :
        m_loadThread(this),
        m_callback(nullptr)
{
}

void SfzLoader::setSfzFile(File *pNewSfzFile)
{
    m_sfzFile = *pNewSfzFile;
}

void SfzLoader::loadSounds(int iNumInstances /*= 1*/, bool bUseLoaderThread /*= false*/, std::function<void()> *callback /*= nullptr*/)
{
    m_iNumInstances = iNumInstances;
    if (callback)
        m_callback = *callback;
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