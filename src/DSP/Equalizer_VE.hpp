#pragma once
#include <juce_dsp/juce_dsp.h>

/**
 * @class Equalizer_VE
 * @brief Ein parametrischer Equalizer mit mehreren Bändern (LowPass, LowShelf, Peak, HighShelf, HighPass).
 *
 * Diese Klasse implementiert einen Mehrband-EQ mit fünf Filtertypen pro Kanal, basierend auf JUCEs DSP-Modulen.
 * Die Filter können dynamisch aktualisiert und in einem Audioblock verarbeitet werden.
 */
class Equalizer_VE
{
public:
    /**
     * @brief Standardkonstruktor.
     */
    Equalizer_VE() = default;

    /**
     * @brief Standarddestruktor.
     */
    ~Equalizer_VE() = default;

    /**
     * @brief Bereitet den Equalizer für die Verarbeitung vor.
     *
     * @param sampleRate Die Abtastrate des Audioprozesses (in Hz).
     * @param samplesPerBlock Die erwartete Blockgröße.
     * @param numChannels Anzahl der Audiokanäle.
     */
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

    /**
     * @brief Aktualisiert die Filter-Koeffizienten basierend auf den übergebenen Parametern.
     *
     * Diese Methode konfiguriert alle fünf Filtertypen pro Kanal:
     * LowPass, LowShelf, Peak (Mid), HighShelf und HighPass.
     *
     * @param lowPassFreq Frequenz des LowPass-Filters (Hz).
     * @param lowFreq Frequenz des LowShelf-Filters (Hz).
     * @param lowGain Gain des LowShelf-Filters (dB).
     * @param lowQ Q-Faktor des LowShelf-Filters.
     * @param midFreq Frequenz des Peak-Filters (Hz).
     * @param midGain Gain des Peak-Filters (dB).
     * @param midQ Q-Faktor des Peak-Filters.
     * @param highFreq Frequenz des HighShelf-Filters (Hz).
     * @param highGain Gain des HighShelf-Filters (dB).
     * @param highQ Q-Faktor des HighShelf-Filters.
     * @param highPassFreq Frequenz des HighPass-Filters (Hz).
     */
    void updateFilters(float lowPassFreq,
                       float lowFreq, float lowGain, float lowQ,
                       float midFreq, float midGain, float midQ,
                       float highFreq, float highGain, float highQ,
                       float highPassFreq);

    /**
     * @brief Verarbeitet einen Audiobuffer mithilfe der aktuellen Filtereinstellungen.
     *
     * @param buffer Der Audiobuffer, der bearbeitet werden soll.
     */
    void processBlock(juce::AudioBuffer<float>& buffer);

private:
    /**
     * @brief Alias für ein einzelnes IIR-Filter.
     */
    using Filter = juce::dsp::IIR::Filter<float>;

    /**
     * @brief Verarbeitet fünf Filtertypen in einer Kette:
     * 0: LowPass, 1: LowShelf, 2: MidPeak, 3: HighShelf, 4: HighPass
     */
    using Chain = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter, Filter>;

    /**
     * @brief Eine Filterkette pro Kanal (z. B. für Stereo zwei Chains).
     */
    std::vector<Chain> filterChains;

    /**
     * @brief Die aktuelle Abtastrate (Hz).
     */
    double currentSampleRate = 44100.0;
};
