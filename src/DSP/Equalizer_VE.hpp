#pragma once
#include <juce_dsp/juce_dsp.h>

class Equalizer_VE
{
public:
    Equalizer_VE() = default;
    ~Equalizer_VE() = default;

    // Bereitet den Equalizer vor (Sample-Rate, Blockgröße, Kanäle)
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

    /** Aktualisiert die Filter-Koeffizienten
     *
     * @param lowPassFreq Frequenz des Lowpass-Filters
     * @param lowFreq Frequenz des LowShelf-Filters
     * @param lowGain Gain in dB
     * @param lowQ Q-Faktor
     * @param midFreq Mid-Frequenz
     * @param midGain Gain in dB
     * @param midQ Q-Faktor
     * @param highFreq HighShelf-Frequenz
     * @param highGain Gain in dB
     * @param highQ Q-Faktor
     * @param highPassFreq Frequenz des Highpass-Filters
     */
    void updateFilters(float lowPassFreq,
                       float lowFreq, float lowGain, float lowQ,
                       float midFreq, float midGain, float midQ,
                       float highFreq, float highGain, float highQ,
                       float highPassFreq);

    // Verarbeitet den Audio-Block
    void processBlock(juce::AudioBuffer<float>& buffer);

private:
    using Filter = juce::dsp::IIR::Filter<float>;
    // 0: LowPass, 1: LowShelf, 2: MidPeak, 3: HighShelf, 4: HighPass
    using Chain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter>;

    std::vector<Chain> filterChains;
    double currentSampleRate = 44100.0;
};