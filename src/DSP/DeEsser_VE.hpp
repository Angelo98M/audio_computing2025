#pragma once

#include <juce_dsp/juce_dsp.h>

class DeEsser_VE
{
public:
    DeEsser_VE();

    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);

    void setThreshold(float newValue);  // dB
    void setFrequency(float newValue);  // Hz
    void setAttack(float newValue);     // ms
    void setRelease(float newValue);    // ms
    void setBypassed(bool shouldBypass);

private:
    juce::dsp::IIR::Filter<float> sibilanceBandFilter;
    juce::dsp::Compressor<float> sibilanceCompressor;

    float currentFrequency = 6000.0f; // Startfrequenz
    bool bypassed = false;
};
