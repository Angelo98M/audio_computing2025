#include "VocalEnhancerEditor.hpp"

VocalEnhancerEditor::VocalEnhancerEditor(VocalEnhancerProcessor& p)
    : AudioProcessorEditor(p), processorRef(p)
{
    // === Compressor ===
    configureSlider(compThresholdSlider);
    configureSlider(compRatioSlider);
    configureSlider(compAttackSlider);
    configureSlider(compReleaseSlider);

    compThresholdAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "compThreshold", compThresholdSlider);
    compRatioAttachment     = std::make_unique<SliderAttachment>(processorRef.parameters, "compRatio", compRatioSlider);
    compAttackAttachment    = std::make_unique<SliderAttachment>(processorRef.parameters, "compAttack", compAttackSlider);
    compReleaseAttachment   = std::make_unique<SliderAttachment>(processorRef.parameters, "compRelease", compReleaseSlider);

    // === De-Esser ===
    configureSlider(deEsserThresholdSlider);
    configureSlider(deEsserFreqSlider);

    deEsserThresholdAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "deessThreshold", deEsserThresholdSlider);
    deEsserFreqAttachment      = std::make_unique<SliderAttachment>(processorRef.parameters, "deessFreq", deEsserFreqSlider);

    // === Equalizer (Gain Only) ===
    configureSlider(eqLowGainSlider);
    configureSlider(eqMidGainSlider);
    configureSlider(eqHighGainSlider);

    eqLowGainAttachment  = std::make_unique<SliderAttachment>(processorRef.parameters, "eqLowGain", eqLowGainSlider);
    eqMidGainAttachment  = std::make_unique<SliderAttachment>(processorRef.parameters, "eqMidGain", eqMidGainSlider);
    eqHighGainAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "eqHighGain", eqHighGainSlider);

    // === Exciter ===
    configureSlider(exciterIntensitySlider);
    configureSlider(exciterMixSlider);

    exciterIntensityAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "exciterIntensity", exciterIntensitySlider);
    exciterMixAttachment       = std::make_unique<SliderAttachment>(processorRef.parameters, "exciterMix", exciterMixSlider);


    // === File Browser ===
    addAndMakeVisible(loadFileButton);
    loadFileButton.onClick = [this]() { openFileChooser(); };

    // === Play ===
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);

    playButton.onClick = [this]() {
        if (processorRef.isFileLoaded())
            processorRef.startPlayback();
    };

    stopButton.onClick = [this]() {
        processorRef.stopPlayback();
    };


    // === Add all sliders to the editor ===
    auto sliders = {
        &compThresholdSlider, &compRatioSlider, &compAttackSlider, &compReleaseSlider,
        &deEsserThresholdSlider, &deEsserFreqSlider,
        &eqLowGainSlider, &eqMidGainSlider, &eqHighGainSlider,
        &exciterIntensitySlider, &exciterMixSlider
    };

    for (auto* s : sliders)
        addAndMakeVisible(*s);

    setSize(1000, 600);
}

void VocalEnhancerEditor::configureSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    slider.setRange(-24.0f, 24.0f);
}

void VocalEnhancerEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawFittedText("Voice Enhancer", getLocalBounds().removeFromTop(30), juce::Justification::centred, 1);
}

void VocalEnhancerEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    auto top = bounds.removeFromTop(150);
    auto middle = bounds.removeFromTop(120);
    auto bottom = bounds.removeFromTop(90);
    auto fotter = bounds;

    auto layoutRow = [](auto& area, auto& s1, auto& s2, auto& s3, auto& s4)
    {
        auto width = area.getWidth() / 4;
        s1.setBounds(area.removeFromLeft(width));
        s2.setBounds(area.removeFromLeft(width));
        s3.setBounds(area.removeFromLeft(width));
        s4.setBounds(area.removeFromLeft(width));
    };

    auto bottomBar = bounds.removeFromBottom(40);
    playButton.setBounds(bottomBar.removeFromLeft(100));
    stopButton.setBounds(bottomBar.removeFromLeft(100));

    layoutRow(top, compThresholdSlider, compRatioSlider, compAttackSlider, compReleaseSlider);
    layoutRow(middle, deEsserThresholdSlider, deEsserFreqSlider, eqLowGainSlider, eqMidGainSlider);
    layoutRow(bottom, eqHighGainSlider, exciterIntensitySlider, exciterMixSlider, exciterMixSlider);
    layoutRow(fotter,playButton,stopButton,loadFileButton,playButton);// letzter doppelt, falls leer
}

void VocalEnhancerEditor::openFileChooser()
{
    fileChooser = std::make_unique<juce::FileChooser>("Select an audio file...",
                                                      juce::File{},
                                                      "*.wav;*.mp3");

    auto chooserFlags = juce::FileBrowserComponent::openMode
                      | juce::FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& chooser)
    {
        auto result = chooser.getResult();

        if (result.existsAsFile())
        {
            juce::String path = result.getFullPathName();
            juce::Logger::writeToLog("Selected file: " + path);

            processorRef.loadFile(result);
        }
    });
}
