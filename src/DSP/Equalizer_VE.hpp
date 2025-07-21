#pragma once
#include <juce_dsp/juce_dsp.h>

class Equalizer_VE
{
public:
    Equalizer_VE() = default;
    ~Equalizer_VE() = default;

    // Bereitet den Equalizer vor (Sample-Rate, Blockgröße, Kanäle)
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

    // Aktualisiert die Filter-Koeffizienten (Parameter werden von außen übergeben)
    void updateFilters(float lowFreq, float lowGain, float lowQ,
                       float midFreq, float midGain, float midQ,
                       float highFreq, float highGain, float highQ);

    // Verarbeitet den Audio-Block
    void processBlock(juce::AudioBuffer<float>& buffer);

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    using Chain = juce::dsp::ProcessorChain<Filter, Filter, Filter>;

    Chain filterChain;
    double currentSampleRate = 44100.0;