//
// Created by Angel on 23.07.2025.
//

#pragma once

#include <juce_dsp/juce_dsp.h>

class Exciter_VE
{
public:
    Exciter_VE();

    void prepare(double sampleRate, int samplesPerBlock, int numChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void setIntensity(float newValue); // 0.0 – 1.0
    void setMix(float newValue);       // 0.0 – 1.0

private:
    float mix = 0.5f;       // Dry/Wet-Mix
    float intensity = 0.7f; // Verzerrungsintensität

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> highPassFilter;

    juce::AudioBuffer<float> exciterBuffer;

    void applySaturation(juce::AudioBuffer<float>& buffer);
};