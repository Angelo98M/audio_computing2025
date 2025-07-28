#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
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

private:
    // DSP-Module
    Equalizer_VE equalizer;
    Exciter_VE exciter;
    Compressor_VE compressor;
    DeEsser_VE deEsser;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalEnhancerProcessor)
};
