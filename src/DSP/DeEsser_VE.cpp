#include "DeEsser_VE.hpp"

DeEsser_VE::DeEsser_VE()
{
    sibilanceCompressor.setThreshold(-20.0f); // dB
    sibilanceCompressor.setRatio(4.0f);       // 4:1
    sibilanceCompressor.setAttack(5.0f);      // ms
    sibilanceCompressor.setRelease(100.0f);   // ms
}

void DeEsser_VE::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(numChannels);

    sibilanceBandFilters.clear();
    sibilanceBandFilters.resize(numChannels);

    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, currentFrequency);

    for (auto& filter : sibilanceBandFilters)
    {
        filter.reset();
        filter.coefficients = coeffs; // Shared Pointer ok
    }
    sibilanceCompressor.prepare(spec);
}

void DeEsser_VE::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;

    juce::AudioBuffer<float> sibilantBuffer;
    sibilantBuffer.makeCopyOf(buffer);

    juce::dsp::AudioBlock<float> block(sibilantBuffer);
    for (size_t ch = 0; ch < block.getNumChannels(); ++ch)
    {
        auto channelBlock = block.getSingleChannelBlock(ch);
        juce::dsp::ProcessContextReplacing<float> context(channelBlock);
        sibilanceBandFilters[ch].process(context);
    }
    juce::dsp::ProcessContextReplacing<float> context(block);


    sibilanceCompressor.process(context);

    // 3. Abziehen vom Originalsignal (De-Essing)
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        buffer.addFrom(ch, 0, sibilantBuffer, ch, 0, buffer.getNumSamples(), -1.0f);
}

void DeEsser_VE::setThreshold(float newValue)
{
    sibilanceCompressor.setThreshold(juce::jlimit(-60.0f, 0.0f, newValue));
}

void DeEsser_VE::setFrequency(float newValue)
{
    currentFrequency = juce::jlimit(2000.0f, 12000.0f, newValue);
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(44100.0f, currentFrequency);
    for (auto& filter : sibilanceBandFilters)
    {
        filter.coefficients = coeffs;  // shared_ptr kopieren
    }
}

void DeEsser_VE::setAttack(float newValue)
{
    sibilanceCompressor.setAttack(newValue);
}

void DeEsser_VE::setRelease(float newValue)
{
    sibilanceCompressor.setRelease(newValue);
}

void DeEsser_VE::setBypassed(bool shouldBypass)
{
    bypassed = shouldBypass;
}
