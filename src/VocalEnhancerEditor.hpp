#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "VocalEnhancerProcessor.hpp"
#include "WaveformDisplay.hpp"
#include "LevelMeterComponent.hpp"

/**
 * @class VocalEnhancerEditor
 * @brief Die grafische Benutzeroberfläche (GUI) für das VocalEnhancer-Plugin.
 *
 * Diese Klasse definiert alle Steuerelemente, Anzeigen und Verbindungen
 * zum AudioProcessor. Sie umfasst Komponenten für Kompressor, De-Esser, Equalizer,
 * Exciter, ADSR, LevelMeter und Dateiverwaltung.
 */
class VocalEnhancerEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    /**
     * @brief Konstruktor.
     * @param processor Referenz auf den zugehörigen AudioProcessor.
     */
    VocalEnhancerEditor(VocalEnhancerProcessor& processor);

    /** @brief Destruktor. */
    ~VocalEnhancerEditor() override = default;

    /**
     * @brief Zeichnet die GUI.
     * @param g Das JUCE-Grafikobjekt zum Zeichnen.
     */
    void paint(juce::Graphics& g) override;

    /**
     * @brief Legt Position und Größe aller GUI-Komponenten fest.
     */
    void resized() override;

private:
    /** @brief Referenz auf den AudioProcessor. */
    VocalEnhancerProcessor& processorRef;

    // === Sliders ===

    /// Kompressor-Regler
    juce::Slider compThresholdSlider, compRatioSlider, compAttackSlider, compReleaseSlider;

    /// De-Esser-Regler
    juce::Slider deEsserThresholdSlider, deEsserFreqSlider;

    /// Equalizer-Regler
    juce::Slider eqLowGainSlider, eqMidGainSlider, eqHighGainSlider, eqLowPassSlider, eqHighPassSlider;

    /// Exciter-Regler
    juce::Slider exciterIntensitySlider, exciterMixSlider;

    // === Attachments ===

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    // Kompressor-Attachments
    std::unique_ptr<SliderAttachment> compThresholdAttachment, compRatioAttachment;
    std::unique_ptr<SliderAttachment> compAttackAttachment, compReleaseAttachment;

    // De-Esser-Attachments
    std::unique_ptr<SliderAttachment> deEsserThresholdAttachment, deEsserFreqAttachment;

    // EQ-Attachments
    std::unique_ptr<SliderAttachment> eqLowGainAttachment, eqMidGainAttachment, eqHighGainAttachment;
    std::unique_ptr<SliderAttachment> eqLowPassAttachment, eqHighPassAttachment;

    // Exciter-Attachments
    std::unique_ptr<SliderAttachment> exciterIntensityAttachment, exciterMixAttachment;

    // === Gruppen (visuelle Trennung) ===
    juce::GroupComponent compressorGroup { "Compressor Group", "Compressor" };
    juce::GroupComponent deEsserGroup   { "DeEsser Group", "De-Esser" };
    juce::GroupComponent eqGroup        { "EQ Group", "Equalizer" };
    juce::GroupComponent exciterGroup   { "Exciter Group", "Exciter" };
    juce::GroupComponent adsrGroup      { "ADSR Group", "ADSR" };

    // === Labels ===

    /// Labels für Kompressor
    juce::Label compThresholdLabel, compRatioLabel, compAttackLabel, compReleaseLabel;

    /// Labels für De-Esser
    juce::Label deEsserThresholdLabel, deEsserFreqLabel;

    /// Labels für EQ
    juce::Label eqLowGainLabel, eqMidGainLabel, eqHighGainLabel, eqLowPassLabel, eqHighPassLabel;

    /// Labels für Exciter
    juce::Label exciterIntensityLabel, exciterMixLabel;

    /**
     * @brief Konfiguriert grundlegende Eigenschaften eines Sliders (Look & Feel, Farbe etc.).
     * @param slider Der zu konfigurierende Slider.
     */
    void configureSlider(juce::Slider& slider);

    // === Dateiauswahl ===

    /// Button zum Laden einer Datei
    juce::TextButton loadFileButton { "Datei Laden" };
    std::unique_ptr<juce::FileChooser> fileChooser;

    /// Button zum Speichern eines bearbeiteten Files
    juce::TextButton saveButton { "Speichern" };
    std::unique_ptr<juce::FileChooser> saveChooser;

    /**
     * @brief Öffnet den Dateidialog zum Laden einer Audiodatei.
     */
    void openFileChooser();

    /**
     * @brief Exportiert das bearbeitete Audiosignal.
     */
    void exportProcessedFile();

    // === Playback-Steuerung ===
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };

    // === Profile (Presets) ===
    juce::ComboBox profileComboBox;
    juce::TextButton reloadProfilesButton { "Neu Laden" };
    juce::TextButton saveProfileButton { "Profil speichern" };

    /**
     * @brief Aktualisiert die Profil-Liste im Dropdown-Menü.
     */
    void reloadProfileList();

    // === Wellenformanzeige ===
    WaveformDisplay waveformDisplay;

    /**
     * @brief Timer-Callback zur regelmäßigen Aktualisierung (z. B. Waveform, Meter).
     */
    void timerCallback() override;

    /**
     * @brief Aktualisiert die grafische Darstellung der ADSR-Hüllkurve.
     */
    void updateADSRVisual();

    // === ADSR-Regler ===
    juce::Slider adsrAttackSlider, adsrDecaySlider, adsrSustainSlider, adsrReleaseSlider;
    juce::Label adsrAttackLabel, adsrDecayLabel, adsrSustainLabel, adsrReleaseLabel;

    std::unique_ptr<SliderAttachment> adsrAttackAttachment, adsrDecayAttachment;
    std::unique_ptr<SliderAttachment> adsrSustainAttachment, adsrReleaseAttachment;

    // === LevelMeter ===
    LevelMeterComponent levelMeter;

    // === Audio-Dateiverwaltung ===
    juce::AudioFormatManager formatManager;

    /**
     * @brief Gibt das Verzeichnis zurück, in dem Presets gespeichert werden.
     * @return JUCE File-Objekt mit Pfad.
     */
    juce::File getProfileDirectory() const;

    /**
     * @brief Gibt eine Liste aller verfügbaren Preset-Dateien zurück.
     * @return Vektor von JUCE File-Objekten.
     */
    std::vector<juce::File> getAvailableProfiles() const;

    /**
     * @brief Speichert das aktuelle Profil unter einem gegebenen Namen.
     * @param profileName Name des Profils.
     */
    void saveProfileWithName(const juce::String& profileName);

    /**
     * @brief Lädt ein Profil anhand des Namens.
     * @param profileName Name des zu ladenden Profils.
     */
    void loadProfileFromName(const juce::String& profileName);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalEnhancerEditor)
};
