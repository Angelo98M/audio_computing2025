#include "Exciter_VE.hpp"

Exciter_VE::Exciter()
{
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(44100.0, 3000.0); // >3kHz
    *highPassFilter.state = *coeffs;
}

void Exciter_VE::prepare(double sampleRate, int samplesPerBlock, int numChannels)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(numChannels);

    highPassFilter.prepare(spec);
    exciterBuffer.setSize(numChannels, samplesPerBlock);
}

void Exciter_VE::setIntensity(float newValue)
{
    intensity = juce::jlimit(0.0f, 1.0f, newValue);
}

void Exciter_VE::setMix(float newValue)
{
    mix = juce::jlimit(0.0f, 1.0f, newValue);
}

void Exciter_VE::applySaturation(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* data = buffer.getWritePointer(channel);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            // Sanfte Saturation / Waveshaping
            float x = data[i];
            data[i] = std::tanh(x * (1.0f + intensity * 5.0f));
        }
    }
}

void Exciter_VE::processBlock(juce::AudioBuffer<float>& buffer)
{
    exciterBuffer.makeCopyOf(buffer);

    // Highpass > 3kHz
    juce::dsp::AudioBlock<float> block(exciterBuffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    highPassFilter.process(context);

    // Verzerren
    applySaturation(exciterBuffer);

    // Dry/Wet-Mix
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        buffer.addFrom(channel, 0, exciterBuffer, channel, 0, buffer.getNumSamples(), mix);
    }
}
