#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * @class WaveformDisplay
 * @brief Komponente zur Darstellung eines Audiopuffers als Wellenform mit Playhead und ADSR-Darstellung.
 *
 * Diese Komponente visualisiert den Audiopuffer als Wellenform und zeigt optional
 * die aktuelle Position des Playheads sowie eine vereinfachte grafische Darstellung
 * der ADSR-Hüllkurve.
 */
class WaveformDisplay : public juce::Component
{
public:
    /**
     * @brief Konstruktor.
     * @param isStandalone Gibt an, ob die Anwendung im Standalone-Modus läuft.
     */
    WaveformDisplay(bool isStandalone);

    /**
     * @brief Setzt den anzuzeigenden Audiopuffer.
     * @param bufferToUse Der Audiopuffer, der kopiert und dargestellt werden soll.
     */
    void setAudioBuffer(const juce::AudioBuffer<float>& bufferToUse)
    {
        audioBuffer.makeCopyOf(bufferToUse);
        repaint();
    }

    /**
     * @brief Zeichnet die Wellenform.
     * @param g Das JUCE-Grafikobjekt.
     */
    void paint(juce::Graphics& g) override;

    /**
     * @brief Wird aufgerufen, wenn die Komponente neu skaliert wird (nicht verwendet).
     */
    void resized() override {}

    /**
     * @brief Setzt die aktuelle Position des Wiedergabekopfs (in Samples).
     * @param samplePosition Position in Samples relativ zum Audiopuffer.
     */
    void setPlayheadPosition(float samplePosition)
    {
        playheadPosition = samplePosition;
        repaint();
    }

    /**
     * @brief Setzt die ADSR-Werte für eine visuelle Darstellung.
     * @param a Attack-Zeit in Millisekunden.
     * @param d Decay-Zeit in Millisekunden.
     * @param s Sustain-Level (0.0–1.0).
     * @param r Release-Zeit in Millisekunden.
     */
    void setADSR(float a, float d, float s, float r)
    {
        attackMs = a;
        decayMs = d;
        sustainLevel = s;
        releaseMs = r;
        repaint();
    }

    /**
     * @brief Gibt an, ob ein gültiger Audiopuffer vorhanden ist.
     * @return true, wenn der Puffer Samples enthält.
     */
    bool hasBuffer();

private:
    juce::AudioBuffer<float> audioBuffer; ///< Interner Audiopuffer (Kopie).
    float playheadPosition = 0;           ///< Aktuelle Wiedergabeposition (in Samples).
    float attackMs = 100.0f;              ///< Attack-Zeit in Millisekunden.
    float decayMs = 100.0f;               ///< Decay-Zeit in Millisekunden.
    float sustainLevel = 0.8f;            ///< Sustain-Level (0.0–1.0).
    float releaseMs = 200.0f;             ///< Release-Zeit in Millisekunden.
    bool isStandalone = false;            ///< Gibt an, ob im Standalone-Modus.
};
