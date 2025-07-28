#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "VocalEnhancerProcessor.hpp"

class VocalEnhancerEditor : public juce::AudioProcessorEditor
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

    void configureSlider(juce::Slider& slider);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalEnhancerEditor)
};
