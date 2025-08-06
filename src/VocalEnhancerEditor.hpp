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

    /**
     * @brief Destruktor.
     */
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
    /**
     * @brief Referenz auf den AudioProcessor.
     */
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

    /// Typalias für Slider-Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    /// Kompressor-Attachments
    std::unique_ptr<SliderAttachment> compThresholdAttachment, compRatioAttachment;
    std::unique_ptr<SliderAttachment> compAttackAttachment, compReleaseAttachment;

    /// De-Esser-Attachments
    std::unique_ptr<SliderAttachment> deEsserThresholdAttachment, deEsserFreqAttachment;

    /// EQ-Attachments
    std::unique_ptr<SliderAttachment> eqLowGainAttachment, eqMidGainAttachment, eqHighGainAttachment;
    std::unique_ptr<SliderAttachment> eqLowPassAttachment, eqHighPassAttachment;

    /// Exciter-Attachments
    std::unique_ptr<SliderAttachment> exciterIntensityAttachment, exciterMixAttachment;

    // === Gruppenkomponenten ===

    /// Gruppierung der GUI-Komponenten
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

    // === Effektein-/ausschalten ===

    /// Toggle-Buttons zur Aktivierung der Effekte
    juce::ToggleButton compressorToggleButton;
    juce::ToggleButton equalizerToggleButton;
    juce::ToggleButton exciterToggleButton;
    juce::ToggleButton deEsserToggleButton;

    /**
     * @brief Konfiguriert grundlegende Eigenschaften eines Sliders.
     * @param slider Der zu konfigurierende Slider.
     */
    void configureSlider(juce::Slider& slider);

    // === Dateiauswahl ===

    /// Button zum Laden einer Datei
    juce::TextButton loadFileButton { "Datei Laden" };
    std::unique_ptr<juce::FileChooser> fileChooser;

    /// Button zum Speichern eines bearbeiteten Audiosignals
    juce::TextButton saveButton { "Speichern" };
    std::unique_ptr<juce::FileChooser> saveChooser;

    /**
     * @brief Öffnet den Dateidialog zum Laden einer Datei.
     */
    void openFileChooser();

    /**
     * @brief Exportiert das bearbeitete Audiosignal als neue Datei.
     */
    void exportProcessedFile();

    // === Wiedergabesteuerung ===

    /// Button zur Wiedergabe
    juce::TextButton playButton { "Play" };

    /// Button zum Stoppen
    juce::TextButton stopButton { "Stop" };

    // === Profile / Presets ===

    /// Dropdown-Menü für Profile
    juce::ComboBox profileComboBox;

    /// Button zum Neuladen der Profile
    juce::TextButton reloadProfilesButton { "Neu Laden" };

    /// Button zum Speichern eines Profils
    juce::TextButton saveProfileButton { "Profil speichern" };

    /**
     * @brief Aktualisiert die Liste der Profile im Dropdown.
     */
    void reloadProfileList();

    // === Wellenformanzeige ===

    /// Anzeige für die Wellenform
    WaveformDisplay waveformDisplay;

    /**
     * @brief Timer-Callback zur regelmäßigen Aktualisierung der GUI.
     */
    void timerCallback() override;

    /**
     * @brief Aktualisiert die visuelle Darstellung der ADSR-Kurve.
     */
    void updateADSRVisual();

    // === ADSR-Regler und Attachments ===

    /// Regler für Attack, Decay, Sustain, Release
    juce::Slider adsrAttackSlider, adsrDecaySlider, adsrSustainSlider, adsrReleaseSlider;

    /// Labels für ADSR
    juce::Label adsrAttackLabel, adsrDecayLabel, adsrSustainLabel, adsrReleaseLabel;

    /// Attachments für ADSR
    std::unique_ptr<SliderAttachment> adsrAttackAttachment, adsrDecayAttachment;
    std::unique_ptr<SliderAttachment> adsrSustainAttachment, adsrReleaseAttachment;

    // === Pegelanzeige ===

    /// Komponente zur Anzeige des Pegels
    LevelMeterComponent levelMeter;

    // === Audio-Dateiverwaltung ===

    /// AudioFormatManager zum Laden/Speichern
    juce::AudioFormatManager formatManager;

    /**
     * @brief Gibt das Verzeichnis zurück, in dem Presets gespeichert werden.
     * @return JUCE File-Objekt mit dem Pfad zum Profilordner.
     */
    juce::File getProfileDirectory() const;

    /**
     * @brief Gibt eine Liste aller verfügbaren Preset-Dateien zurück.
     * @return Vektor mit Preset-Dateien.
     */
    std::vector<juce::File> getAvailableProfiles() const;

    /**
     * @brief Speichert das aktuelle Profil unter einem gegebenen Namen.
     * @param profileName Name des neuen Profils.
     */
    void saveProfileWithName(const juce::String& profileName);

    /**
     * @brief Lädt ein Profil anhand des übergebenen Namens.
     * @param profileName Name des zu ladenden Profils.
     */
    void loadProfileFromName(const juce::String& profileName);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalEnhancerEditor)
};
