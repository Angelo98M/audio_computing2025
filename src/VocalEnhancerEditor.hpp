#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "VocalEnhancerProcessor.hpp"
#include "WaveformDisplay.hpp"
#include "LevelMeterComponent.hpp"


class VocalEnhancerEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    VocalEnhancerEditor(VocalEnhancerProcessor&);
    ~VocalEnhancerEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;


private:
    VocalEnhancerProcessor& processorRef;

    // === Sliders ===
    juce::Slider compThresholdSlider, compRatioSlider, compAttackSlider, compReleaseSlider;
    juce::Slider deEsserThresholdSlider, deEsserFreqSlider;
    juce::Slider eqLowGainSlider, eqMidGainSlider, eqHighGainSlider;
    juce::Slider exciterIntensitySlider, exciterMixSlider;

    // === Attachments (ValueTree Binding) ===
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<SliderAttachment> compThresholdAttachment, compRatioAttachment;
    std::unique_ptr<SliderAttachment> compAttackAttachment, compReleaseAttachment;

    std::unique_ptr<SliderAttachment> deEsserThresholdAttachment, deEsserFreqAttachment;

    std::unique_ptr<SliderAttachment> eqLowGainAttachment, eqMidGainAttachment, eqHighGainAttachment;

    std::unique_ptr<SliderAttachment> exciterIntensityAttachment, exciterMixAttachment;

    juce::GroupComponent compressorGroup { "Compressor Group", "Compressor" };
    juce::GroupComponent deEsserGroup   { "DeEsser Group", "De-Esser" };
    juce::GroupComponent eqGroup        { "EQ Group", "Equalizer" };
    juce::GroupComponent exciterGroup   { "Exciter Group", "Exciter" };
    juce::GroupComponent adsrGroup   { "Test Group", "Test" };

    // === Labels ===
    juce::Label compThresholdLabel, compRatioLabel, compAttackLabel, compReleaseLabel;
    juce::Label deEsserThresholdLabel, deEsserFreqLabel;
    juce::Label eqLowGainLabel, eqMidGainLabel, eqHighGainLabel;
    juce::Label exciterIntensityLabel, exciterMixLabel;

    void configureSlider(juce::Slider& slider);


    // === File Browser ===
    juce::TextButton loadFileButton { "Datei Laden" };
    std::unique_ptr<juce::FileChooser> fileChooser;
    void openFileChooser();

    juce::TextButton saveButton { "Speichern" };
    std::unique_ptr<juce::FileChooser> saveChooser;
    void exportProcessedFile();

    // === Play ===
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };

    juce::ComboBox profileComboBox;
    juce::TextButton reloadProfilesButton { "Neu Laden" };
    juce::TextButton saveProfileButton { "Profil speichern" };
    void reloadProfileList();

    WaveformDisplay waveformDisplay;
    void timerCallback();
    void updateADSRVisual();

    juce::Slider adsrAttackSlider, adsrDecaySlider, adsrSustainSlider, adsrReleaseSlider;
    juce::Label adsrAttackLabel, adsrDecayLabel, adsrSustainLabel, adsrReleaseLabel;

    std::unique_ptr<SliderAttachment> adsrAttackAttachment, adsrDecayAttachment, adsrSustainAttachment, adsrReleaseAttachment;

    LevelMeterComponent levelMeter;












    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalEnhancerEditor)
};
