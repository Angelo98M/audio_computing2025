#pragma once

#include <juce_dsp/juce_dsp.h>

/**
 * @class Exciter_VE
 * @brief Ein einfacher Exciter zur Anreicherung des Signals mit harmonischen Obertönen.
 *
 * Der Exciter filtert hohe Frequenzen heraus, sättigt diese nichtlinear und mischt das Ergebnis
 * wieder dem Originalsignal bei. Dadurch entsteht ein brillanterer Klang, der sich besonders
 * für Vocals, Drums oder Mastering eignet.
 */
class Exciter_VE
{
public:
    /**
     * @brief Konstruktor der Exciter_VE-Klasse.
     */
    Exciter_VE();

    /**
     * @brief Bereitet den Exciter für die Verarbeitung vor.
     *
     * @param sampleRate Die Abtastrate des Audioprozesses (in Hz).
     * @param samplesPerBlock Die maximale Blockgröße für die Verarbeitung.
     * @param numChannels Die Anzahl der Audiokanäle.
     */
    void prepare(double sampleRate, int samplesPerBlock, int numChannels);

    /**
     * @brief Verarbeitet den übergebenen Audiobuffer mit Exciter-Effekt.
     *
     * @param buffer Der Audiobuffer, der bearbeitet werden soll.
     */
    void processBlock(juce::AudioBuffer<float>& buffer);

    /**
     * @brief Setzt die Intensität der Sättigung.
     *
     * @param newValue Ein Wert zwischen 0.0 und 1.0 zur Steuerung der Verzerrung.
     */
    void setIntensity(float newValue);

    /**
     * @brief Setzt den Dry/Wet-Mix.
     *
     * @param newValue Ein Wert zwischen 0.0 (nur Originalsignal) und 1.0 (nur bearbeitetes Signal).
     */
    void setMix(float newValue);

    /**
     * @brief Aktiviert oder deaktiviert den Exciter-Effekt.
     *
     * @param shouldBypass Wenn true, wird der Effekt umgangen (Bypass).
     */
    void setBypass(bool shouldBypass);

private:
    /** @brief Verhältnis zwischen unbearbeitetem und bearbeitetem Signal. */
    float mix = 0.5f;

    /** @brief Stärke der Sättigung (nichtlineare Verzerrung). */
    float intensity = 0.7f;

    /**
     * @brief Hochpassfilter zum Isolieren des oberen Frequenzbereichs,
     * auf den der Exciter-Effekt angewendet wird.
     */
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                   juce::dsp::IIR::Coefficients<float>> highPassFilter;

    /** @brief Interner Buffer zur Verarbeitung des Exciter-Signals. */
    juce::AudioBuffer<float> exciterBuffer;

    /**
     * @brief Wendet eine nichtlineare Sättigung auf den übergebenen Buffer an.
     *
     * Diese Methode erzeugt harmonische Obertöne basierend auf der eingestellten Intensität.
     *
     * @param buffer Der Audiobuffer, der verzerrt werden soll.
     */
    void applySaturation(juce::AudioBuffer<float>& buffer);

    /** @brief Gibt an, ob der Exciter-Effekt aktuell umgangen wird. */
    bool bypassed = false;
};
