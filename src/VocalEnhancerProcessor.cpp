#include "VocalEnhancerProcessor.hpp"
#include "VocalEnhancerEditor.hpp"

VocalEnhancerProcessor::VocalEnhancerProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

void VocalEnhancerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const auto numChannels = getTotalNumInputChannels();

    equalizer.prepare(sampleRate, samplesPerBlock, numChannels);
    compressor.prepare(sampleRate, samplesPerBlock, numChannels);
    deEsser.prepare(sampleRate, samplesPerBlock, numChannels);
    exciter.prepare(sampleRate, samplesPerBlock, numChannels);
}

void VocalEnhancerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    // Parameter abrufen
    compressor.setThreshold(parameters.getRawParameterValue("compThreshold")->load());
    compressor.setRatio(parameters.getRawParameterValue("compRatio")->load());
    compressor.setAttack(parameters.getRawParameterValue("compAttack")->load());
    compressor.setRelease(parameters.getRawParameterValue("compRelease")->load());

    deEsser.setThreshold(parameters.getRawParameterValue("deessThreshold")->load());
    deEsser.setFrequency(parameters.getRawParameterValue("deessFreq")->load());

    equalizer.updateFilters(
        100.0f, // LowFreq (optional extern param)
        parameters.getRawParameterValue("eqLowGain")->load(), 0.7f,
        1000.0f,
        parameters.getRawParameterValue("eqMidGain")->load(), 1.0f,
        8000.0f,
        parameters.getRawParameterValue("eqHighGain")->load(), 0.7f
    );

    exciter.setIntensity(parameters.getRawParameterValue("exciterIntensity")->load());
    exciter.setMix(parameters.getRawParameterValue("exciterMix")->load());

    // Processing Chain
    compressor.processBlock(buffer);
    deEsser.processBlock(buffer);
    equalizer.processBlock(buffer);
    exciter.processBlock(buffer);
}

juce::AudioProcessorEditor* VocalEnhancerProcessor::createEditor()
{
    return new VocalEnhancerEditor(*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout VocalEnhancerProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // === Compressor ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compThreshold", "Comp Threshold", -60.0f, 0.0f, -24.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compRatio",     "Comp Ratio",     1.0f, 20.0f, 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compAttack",    "Comp Attack",    0.1f, 100.0f, 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compRelease",   "Comp Release",   5.0f, 300.0f, 100.0f));

    // === De-Esser ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>("deessThreshold", "De-Esser Threshold", -60.0f, 0.0f, -30.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("deessFreq",      "De-Esser Freq",      2000.0f, 12000.0f, 6000.0f));

    // === Equalizer ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqLowGain",  "EQ Low Gain",  -24.0f, 24.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqMidGain",  "EQ Mid Gain",  -24.0f, 24.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqHighGain", "EQ High Gain", -24.0f, 24.0f, 0.0f));

    // === Exciter ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>("exciterIntensity", "Exciter Intensity", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("exciterMix",       "Exciter Mix",       0.0f, 1.0f, 0.5f));

    return { params.begin(), params.end() };
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalEnhancerProcessor();
}

void VocalEnhancerProcessor::getStateInformation(juce::MemoryBlock& destData) {
}

void VocalEnhancerProcessor::setStateInformation(const void* data, int sizeInBytes) {
}
