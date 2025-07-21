#include "Equalizer_VE.hpp"

Equalizer_VE::Equalizer_VE()
    : parameters(*this, nullptr, "PARAMETERS", {
        // Low Shelf
        std::make_unique<juce::AudioParameterFloat>("lowFreq", "Low Freq", 20.0f, 500.0f, 100.0f),
        std::make_unique<juce::AudioParameterFloat>("lowGain", "Low Gain", -24.0f, 24.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("lowQ", "Low Q", 0.1f, 5.0f, 0.7f),

        // Mid Peak
        std::make_unique<juce::AudioParameterFloat>("midFreq", "Mid Freq", 200.0f, 5000.0f, 1000.0f),
        std::make_unique<juce::AudioParameterFloat>("midGain", "Mid Gain", -24.0f, 24.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("midQ", "Mid Q", 0.1f, 5.0f, 1.0f),

        // High Shelf
        std::make_unique<juce::AudioParameterFloat>("highFreq", "High Freq", 2000.0f, 20000.0f, 8000.0f),
        std::make_unique<juce::AudioParameterFloat>("highGain", "High Gain", -24.0f, 24.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("highQ", "High Q", 0.1f, 5.0f, 0.7f),
    })
{}

void Equalizer_VE::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, getMainBusNumOutputChannels() };
    filterChain.prepare(spec);
    updateFilters();
}

void Equalizer_VE::updateFilters()
{
    auto lowFreq = parameters.getRawParameterValue("lowFreq")->load();
    auto lowGain = parameters.getRawParameterValue("lowGain")->load();
    auto lowQ = parameters.getRawParameterValue("lowQ")->load();

    auto midFreq = parameters.getRawParameterValue("midFreq")->load();
    auto midGain = parameters.getRawParameterValue("midGain")->load();
    auto midQ = parameters.getRawParameterValue("midQ")->load();

    auto highFreq = parameters.getRawParameterValue("highFreq")->load();
    auto highGain = parameters.getRawParameterValue("highGain")->load();
    auto highQ = parameters.getRawParameterValue("highQ")->load();

    auto lowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), lowFreq, lowQ, juce::Decibels::decibelsToGain(lowGain));
    auto midCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), midFreq, midQ, juce::Decibels::decibelsToGain(midGain));
    auto highCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), highFreq, highQ, juce::Decibels::decibelsToGain(highGain));

    *filterChain.get<0>().coefficients = *lowCoeffs;
    *filterChain.get<1>().coefficients = *midCoeffs;
    *filterChain.get<2>().coefficients = *highCoeffs;
}

void Equalizer_VE::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    updateFilters();

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filterChain.process(context);
}

juce::AudioProcessorEditor* Equalizer_VE::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
