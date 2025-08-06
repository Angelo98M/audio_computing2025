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

void Equalizer_VE::updateFilters(float lowPassFreq,
                                 float lowFreq, float lowGain, float lowQ,
                                 float midFreq, float midGain, float midQ,
                                 float highFreq, float highGain, float highQ,
                                 float highPassFreq)
{
    auto lowPassCoeffs  = juce::dsp::IIR::Coefficients<float>::makeLowPass(currentSampleRate, lowPassFreq);
    auto lowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(currentSampleRate, lowFreq, lowQ, juce::Decibels::decibelsToGain(lowGain));
    auto midCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(currentSampleRate, midFreq, midQ, juce::Decibels::decibelsToGain(midGain));
    auto highCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(currentSampleRate, highFreq, highQ, juce::Decibels::decibelsToGain(highGain));
    auto highPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(currentSampleRate, highPassFreq);

    for (auto& chain : filterChains)
    {
        *chain.get<0>().coefficients = *lowPassCoeffs;
        *chain.get<1>().coefficients = *lowCoeffs;
        *chain.get<2>().coefficients = *midCoeffs;
        *chain.get<3>().coefficients = *highCoeffs;
        *chain.get<4>().coefficients = *highPassCoeffs;
    }
}

void Equalizer_VE::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumSamples() == 0 || buffer.getNumChannels() == 0)
        return;
    if (bypassed)
        return;

    const int numChannels = buffer.getNumChannels();
    juce::dsp::AudioBlock<float> fullBlock(buffer);

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto channelBlock = fullBlock.getSingleChannelBlock(ch);
        juce::dsp::ProcessContextReplacing<float> context(channelBlock);
        filterChains[ch].process(context);
    }
}