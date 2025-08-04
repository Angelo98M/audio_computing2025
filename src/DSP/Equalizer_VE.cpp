#include "Equalizer_VE.hpp"

void Equalizer_VE::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(numChannels);

    filterChains.clear();
    filterChains.resize(numChannels);

    for (auto& chain : filterChains)
        chain.prepare(spec);
}

void Equalizer_VE::updateFilters(float lowFreq, float lowGain, float lowQ,
                                float midFreq, float midGain, float midQ,
                                float highFreq, float highGain, float highQ)
{
    auto lowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(currentSampleRate, lowFreq, lowQ, juce::Decibels::decibelsToGain(lowGain));
    auto midCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, midFreq, midQ, juce::Decibels::decibelsToGain(midGain));
    auto highCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(currentSampleRate, highFreq, highQ, juce::Decibels::decibelsToGain(highGain));

    for (auto& chain : filterChains)
    {
        *chain.get<0>().coefficients = *lowCoeffs;
        *chain.get<1>().coefficients = *midCoeffs;
        *chain.get<2>().coefficients = *highCoeffs;
    }
}

void Equalizer_VE::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    juce::dsp::AudioBlock<float> fullBlock(buffer);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto channelBlock = fullBlock.getSingleChannelBlock(ch);
        juce::dsp::ProcessContextReplacing<float> context(channelBlock);
        filterChains[ch].process(context);
    }
}