#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

/**
 * @class LevelMeterComponent
 * @brief Eine einfache visuelle Pegelanzeige für Audiosignale.
 *
 * Diese Komponente zeigt einen vertikalen Pegelbalken an, der auf einem externen,
 * atomar bereitgestellten Float-Wert basiert. Der Wert wird regelmäßig aktualisiert
 * und grafisch als dB-skaliertes Level dargestellt.
 *
 * Typische Verwendung: RMS-/Peak-Anzeige in einem Audio-Plugin.
 */
class LevelMeterComponent : public juce::Component, public juce::Timer
{
public:
    /**
     * @brief Konstruktor.
     *
     * @param levelRef Eine atomare Referenz auf den externen Pegelwert,
     * der in Echtzeit visualisiert werden soll.
     */
    LevelMeterComponent(std::atomic<float>& levelRef) : level(levelRef)
    {
        startTimerHz(30); ///< Aktualisiert Anzeige etwa 30-mal pro Sekunde (30 FPS).
    }

    /**
     * @brief Zeichnet die Pegelanzeige.
     *
     * @param g JUCE-Grafikobjekt für das Rendering.
     *
     * Die Methode zeichnet einen farbigen Pegelbalken basierend auf dem aktuellen dB-Wert.
     * Der Balken reicht von -60 dB (unten) bis 0 dB (oben) und wird in Limegrün dargestellt.
     */
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.fillAll(juce::Colours::black); ///< Hintergrund

        float dB = juce::Decibels::gainToDecibels(currentLevel);
        float normLevel = juce::jmap(dB, -60.0f, 0.0f, 0.0f, 1.0f); ///< dB → normiertes Level

        normLevel = juce::jlimit(0.0f, 1.0f, normLevel); ///< Begrenzung auf gültigen Bereich

        auto levelHeight = bounds.getHeight() * (1.0f - normLevel);
        g.setColour(juce::Colours::limegreen);
        g.fillRect(bounds.withY(levelHeight).withHeight(bounds.getHeight() - levelHeight)); ///< Balken

        g.setColour(juce::Colours::white);
        g.drawRect(bounds, 2.0f); ///< Rahmen
    }

    /**
     * @brief Wird regelmäßig vom Timer aufgerufen, um den Pegelwert zu aktualisieren.
     *
     * Diese Methode liest den aktuellen Pegelwert atomar und triggert ein Repaint der Komponente.
     */
    void timerCallback() override
    {
        currentLevel = level.load(); ///< Atomare Leseoperation
        repaint();                   ///< Anzeige aktualisieren
    }

private:
    /**
     * @brief Atomare Referenz auf den externen Pegelwert.
     *
     * Wird z. B. im Audio-Thread aktualisiert und von der GUI-Komponente gelesen.
     */
    std::atomic<float>& level;

    /**
     * @brief Intern gespeicherter Pegelwert für die Anzeige.
     *
     * Wird im Timer aktualisiert und im Paint-Prozess verwendet.
     */
    float currentLevel = 0.0f;
};
