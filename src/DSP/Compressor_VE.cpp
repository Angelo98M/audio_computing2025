#include "Compressor_VE.hpp"

Compressor_VE::Compressor_VE()
{
    compressor.setThreshold(-24.0f); // dB
    compressor.setRatio(2.0f);       // 2:1
    compressor.setAttack(10.0f);     // ms
    compressor.setRelease(100.0f);   // ms
}

void Compressor_VE::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(numChannels);

    compressor.prepare(spec);
}

void Compressor_VE::processBlock(juce::AudioBuffer<float>& buffer)
{
    if (bypassed)
        return;

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    compressor.process(context);
}

void Compressor_VE::setThreshold(float newValue)
{
    compressor.setThreshold(juce::jlimit(-60.0f, 0.0f, newValue));
}

void Compressor_VE::setRatio(float newValue)
{
    compressor.setRatio(juce::jlimit(1.0f, 20.0f, newValue));
}

void Compressor_VE::setAttack(float newValue)
{
    compressor.setAttack(juce::jlimit(0.1f, 200.0f, newValue));
}

void Compressor_VE::setRelease(float newValue)
{
    compressor.setRelease(juce::jlimit(5.0f, 500.0f, newValue));
}

void Compressor_VE::setBypassed(bool shouldBeBypassed)
{
    bypassed = shouldBeBypassed;
}
