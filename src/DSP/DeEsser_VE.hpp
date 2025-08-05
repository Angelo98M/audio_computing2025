#pragma once

#include <juce_dsp/juce_dsp.h>

/**
 * @class DeEsser_VE
 * @brief Eine einfache De-Esser-Klasse zur Reduzierung von Zischlauten (Sibilanz) in Audiosignalen.
 *
 * Diese Klasse filtert einen spezifischen Hochfrequenzbereich (typischerweise Sibilanzbereich) heraus
 * und verarbeitet diesen separat mit einem Kompressor. Sie basiert auf JUCEs DSP-Modulen.
 */
class DeEsser_VE
{
public:
    /**
     * @brief Konstruktor der DeEsser_VE-Klasse.
     */
    DeEsser_VE();

    /**
     * @brief Initialisiert den De-Esser für die Audioverarbeitung.
     *
     * @param sampleRate Die Abtastrate des Audioprozesses (in Hz).
     * @param samplesPerBlock Die erwartete Blockgröße bei der Verarbeitung.
     * @param numChannels Die Anzahl der Audiokanäle.
     */
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

    /**
     * @brief Verarbeitet den übergebenen Audiobuffer und reduziert Sibilanz.
     *
     * @param buffer Der zu bearbeitende Audiobuffer.
     */
    void processBlock(juce::AudioBuffer<float>& buffer);

    /**
     * @brief Setzt den Threshold-Wert des Sibilanz-Kompressors.
     *
     * @param newValue Der neue Threshold in Dezibel (dB).
     */
    void setThreshold(float newValue);

    /**
     * @brief Legt die Ziel-Frequenz fest, die als Sibilanzbereich behandelt wird.
     *
     * @param newValue Die neue Frequenz in Hertz (Hz), z. B. 6000 Hz.
     */
    void setFrequency(float newValue);

    /**
     * @brief Setzt die Attack-Zeit des internen Kompressors.
     *
     * @param newValue Die Attack-Zeit in Millisekunden.
     */
    void setAttack(float newValue);

    /**
     * @brief Setzt die Release-Zeit des internen Kompressors.
     *
     * @param newValue Die Release-Zeit in Millisekunden.
     */
    void setRelease(float newValue);

    /**
     * @brief Aktiviert oder deaktiviert den De-Esser (Bypass-Modus).
     *
     * @param shouldBypass Wenn true, wird die Verarbeitung umgangen.
     */
    void setBypassed(bool shouldBypass);

private:
    /**
     * @brief Filter zur Isolierung des Sibilanzbereichs in jedem Kanal.
     *
     * Jeder Kanal erhält ein Bandpass- oder Hochpassfilter, um gezielt den Sibilanzbereich zu bearbeiten.
     */
    std::vector<juce::dsp::IIR::Filter<float>> sibilanceBandFilters;

    /**
     * @brief Kompressor, der ausschließlich auf den gefilterten Sibilanzbereich wirkt.
     */
    juce::dsp::Compressor<float> sibilanceCompressor;

    /**
     * @brief Die aktuell eingestellte Ziel-Frequenz für den Sibilanzbereich in Hz.
     */
    float currentFrequency = 6000.0f;

    /**
     * @brief Gibt an, ob der De-Esser derzeit umgangen wird.
     */
    bool bypassed = false;
};
