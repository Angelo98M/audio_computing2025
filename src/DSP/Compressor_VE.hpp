#pragma once

#include <juce_dsp/juce_dsp.h>

class Compressor_VE
{
public:
    Compressor_VE();

    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);

    void setThreshold(float newValue);  // in dB
    void setRatio(float newValue);      // e.g. 2.0 = 2:1
    void setAttack(float newValue);     // in ms
    void setRelease(float newValue);    // in ms
    void setBypassed(bool shouldBeBypassed);

private:
    juce::dsp::Compressor<float> compressor;
    bool bypassed = false;
};
