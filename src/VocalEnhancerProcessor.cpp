#include "VocalEnhancerProcessor.hpp"

VocalEnhancerProcessor::VocalEnhancerProcessor()
    : parameters(*this, nullptr, "PARAMETERS", {
        // Equalizer
        std::make_unique<juce::AudioParameterFloat>("lowFreq", "Low Freq", 20.0f, 500.0f, 100.0f),
        std::make_unique<juce::AudioParameterFloat>("lowGain", "Low Gain", -24.0f, 24.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("lowQ", "Low Q", 0.1f, 5.0f, 0.7f),
        std::make_unique<juce::AudioParameterFloat>("midFreq", "Mid Freq", 200.0f, 5000.0f, 1000.0f),
        std::make_unique<juce::AudioParameterFloat>("midGain", "Mid Gain", -24.0f, 24.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("midQ", "Mid Q", 0.1f, 5.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("highFreq", "High Freq", 2000.0f, 20000.0f, 8000.0f),
        std::make_unique<juce::AudioParameterFloat>("highGain", "High Gain", -24.0f, 24.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("highQ", "High Q", 0.1f, 5.0f, 0.7f),

        // Exciter
        std::make_unique<juce::AudioParameterFloat>("exciterIntensity", "Exciter Intensity", 0.0f, 1.0f, 0.7f),
        std::make_unique<juce::AudioParameterFloat>("exciterMix", "Exciter Mix", 0.0f, 1.0f, 0.5f),

        // Compressor
        std::make_unique<juce::AudioParameterFloat>("compThresh", "Compressor Threshold", -60.0f, 0.0f, -24.0f),
        std::make_unique<juce::AudioParameterFloat>("compRatio", "Compressor Ratio", 1.0f, 20.0f, 4.0f),
        std::make_unique<juce::AudioParameterFloat>("compAttack", "Compressor Attack", 0.1f, 100.0f, 10.0f),
        std::make_unique<juce::AudioParameterFloat>("compRelease", "Compressor Release", 5.0f, 500.0f, 100.0f),

        // De-Esser
        std::make_unique<juce::AudioParameterFloat>("deessFreq", "DeEsser Frequency", 2000.0f, 12000.0f, 6000.0f),
        std::make_unique<juce::AudioParameterFloat>("deessThresh", "DeEsser Threshold", -60.0f, 0.0f, -20.0f),
        std::make_unique<juce::AudioParameterFloat>("deessAttack", "DeEsser Attack", 0.1f, 100.0f, 10.0f),
        std::make_unique<juce::AudioParameterFloat>("deessRelease", "DeEsser Release", 5.0f, 500.0f, 100.0f),
    })
{}

void VocalEnhancerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    int numChannels = getTotalNumOutputChannels();

    equalizer.prepare(sampleRate, samplesPerBlock, numChannels);
    exciter.prepare(sampleRate, samplesPerBlock, numChannels);
    compressor.prepare(sampleRate, samplesPerBlock, numChannels);
    deEsser.prepare(sampleRate, samplesPerBlock, numChannels);

    updateParameters();
}

void VocalEnhancerProcessor::updateParameters()
{
    // Equalizer
    equalizer.updateFilters(
        parameters.getRawParameterValue("lowFreq")->load(),
        parameters.getRawParameterValue("lowGain")->load(),
        parameters.getRawParameterValue("lowQ")->load(),
        parameters.getRawParameterValue("midFreq")->load(),
        parameters.getRawParameterValue("midGain")->load(),
        parameters.getRawParameterValue("midQ")->load(),
        parameters.getRawParameterValue("highFreq")->load(),
        parameters.getRawParameterValue("highGain")->load(),
        parameters.getRawParameterValue("highQ")->load()
    );

    // Exciter
    exciter.setIntensity(parameters.getRawParameterValue("exciterIntensity")->load());
    exciter.setMix(parameters.getRawParameterValue("exciterMix")->load());

    // Compressor
    compressor.setThreshold(parameters.getRawParameterValue("compThresh")->load());
    compressor.setRatio(parameters.getRawParameterValue("compRatio")->load());
    compressor.setAttack(parameters.getRawParameterValue("compAttack")->load());
    compressor.setRelease(parameters.getRawParameterValue("compRelease")->load());

    // De-Esser
    deEsser.setFrequency(parameters.getRawParameterValue("deessFreq")->load());
    deEsser.setThreshold(parameters.getRawParameterValue("deessThresh")->load());
    deEsser.setAttack(parameters.getRawParameterValue("deessAttack")->load());
    deEsser.setRelease(parameters.getRawParameterValue("deessRelease")->load());
}

void VocalEnhancerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    updateParameters();

    deEsser.processBlock(buffer);
    equalizer.processBlock(buffer);
    compressor.processBlock(buffer);
    exciter.processBlock(buffer);
}

juce::AudioProcessorEditor* VocalEnhancerProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void VocalEnhancerProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream stream(destData, true);
    parameters.state.writeToStream(stream);
}

void VocalEnhancerProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
        parameters.replaceState(tree);
}
