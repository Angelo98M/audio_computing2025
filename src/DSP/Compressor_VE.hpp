#pragma once

#include <juce_dsp/juce_dsp.h>

/**
 * @class Compressor_VE
 * @brief Eine einfache Hüllkurven-basiere Kompressor-Klasse, basierend auf JUCEs DSP-Modulen.
 *
 * Diese Klasse kapselt einen JUCE-Kompressor und bietet Funktionen zum Konfigurieren
 * und Bearbeiten eines Audiobuffers mit typischen Parametern wie Threshold, Ratio, Attack und Release.
 */
class Compressor_VE
{
public:
    /**
     * @brief Konstruktor der Compressor_VE-Klasse.
     */
    Compressor_VE();

    /**
     * @brief Bereitet den internen Kompressor auf die Verarbeitung vor.
     *
     * @param sampleRate Die Abtastrate des Audioprozesses (in Hz).
     * @param samplesPerBlock Die erwartete Blockgröße der Audioverarbeitung.
     * @param numChannels Die Anzahl der Kanäle (z. B. 2 für Stereo).
     */
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

    /**
     * @brief Verarbeitet einen Audiobuffer mithilfe des Kompressors.
     *
     * @param buffer Der zu verarbeitende Audiobuffer.
     */
    void processBlock(juce::AudioBuffer<float>& buffer);

    /**
     * @brief Setzt den Threshold-Wert des Kompressors.
     *
     * @param newValue Der neue Threshold-Wert in Dezibel (dB).
     */
    void setThreshold(float newValue);

    /**
     * @brief Setzt das Kompressionsverhältnis.
     *
     * @param newValue Das neue Verhältnis (z. B. 2.0 für 2:1).
     */
    void setRatio(float newValue);

    /**
     * @brief Setzt die Attack-Zeit des Kompressors.
     *
     * @param newValue Die Attack-Zeit in Millisekunden.
     */
    void setAttack(float newValue);

    /**
     * @brief Setzt die Release-Zeit des Kompressors.
     *
     * @param newValue Die Release-Zeit in Millisekunden.
     */
    void setRelease(float newValue);

    /**
     * @brief Aktiviert oder deaktiviert den Bypass-Modus.
     *
     * @param shouldBeBypassed Wenn true, wird der Kompressor umgangen.
     */
    void setBypassed(bool shouldBeBypassed);

private:
    /** @brief Der interne JUCE-Kompressor. */
    juce::dsp::Compressor<float> compressor;

    /** @brief Gibt an, ob der Kompressor derzeit umgangen (bypassed) ist. */
    bool bypassed = false;
};
