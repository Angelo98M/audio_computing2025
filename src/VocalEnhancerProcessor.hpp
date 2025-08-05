#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "DSP/Exciter_VE.hpp"
#include "DSP/DeEsser_VE.hpp"
#include "DSP/Compressor_VE.hpp"
#include  "DSP/Equalizer_VE.hpp"




//==============================================================================
class VocalEnhancerProcessor final : public juce::AudioProcessor
{
public:
    VocalEnhancerProcessor();
    ~VocalEnhancerProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "VoiceEnhancer"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
    void loadFile(const juce::File& audioFile);
    juce::AudioBuffer<float>& getLoadedBuffer() { return loadedBuffer; }
    bool isFileLoaded() const { return fileLoaded; }
    void startPlayback();   // zum Starten
    void stopPlayback();    // zum Stoppen
    float getPlayHeadPosition() const { return playHeadPositon; }
    bool getIsStandalone() const { return isStandalone; }
    juce::AudioBuffer<float> getWriteBuffer(){return writebuffer;}
    const juce::AudioBuffer<float>& getWaveBuffer() const { return waveBuffer; }
    void prepareWriteBuffer();
    double getLoadedSampleRate() const { return loadedSampleRate; }
    juce::File getProfileDirectory() const;
    std::vector<juce::File> getAvailableProfiles() const;
    void saveProfileWithName(const juce::String& profileName);
    void loadProfileFromName(const juce::String& profileName);
    std::atomic<float> currentLevel = 0.0f;
    void loadParametersformXML(std::unique_ptr<juce::XmlElement> xml);
    juce::ValueTree getParameters() {return parameters.copyState();}

private:
    // DSP-Module
    Equalizer_VE equalizer;
    Exciter_VE exciter;
    Compressor_VE compressor;
    DeEsser_VE deEsser;

    // === Read Audio ===
    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> loadedBuffer;
    bool fileLoaded = false;

    // === play ===
    int playPosition = 0;
    float playHeadPositon = 0;
    bool isPlaying = false;

    bool isStandalone = juce::JUCEApplicationBase::isStandaloneApp();

    juce::AudioBuffer<float> writebuffer;
    double loadedSampleRate = 44100.0; // fallback default

    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    bool hasCopiedFromHost = false;
    juce::AudioBuffer<float> waveBuffer;
    bool wasPlaying = false;

    void dspChain(juce::AudioBuffer<float>& buffer);
    void loadParameters();



    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalEnhancerProcessor)
};
