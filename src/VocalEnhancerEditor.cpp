#include "VocalEnhancerEditor.hpp"

VocalEnhancerEditor::VocalEnhancerEditor(VocalEnhancerProcessor& p)
    : AudioProcessorEditor(p), processorRef(p)
{
    startTimerHz(30);
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

    auto setupLabel = [this](juce::Label& label, const juce::String& text)
    {
        label.setText(text, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);
    };

    // === Compressor Labels ===
    setupLabel(compThresholdLabel, "Threshold");
    setupLabel(compRatioLabel, "Ratio");
    setupLabel(compAttackLabel, "Attack");
    setupLabel(compReleaseLabel, "Release");

    // === De-Esser Labels ===
    setupLabel(deEsserThresholdLabel, "Threshold");
    setupLabel(deEsserFreqLabel, "Freq");

    // === EQ Labels ===
    setupLabel(eqLowGainLabel, "Low Gain");
    setupLabel(eqMidGainLabel, "Mid Gain");
    setupLabel(eqHighGainLabel, "High Gain");

    // === Exciter Labels ===
    setupLabel(exciterIntensityLabel, "Intensity");
    setupLabel(exciterMixLabel, "Mix");

    addAndMakeVisible(waveformDisplay);

    for (auto* s : sliders)
        addAndMakeVisible(*s);

    // === Group Components anzeigen ===
    addAndMakeVisible(compressorGroup);
    addAndMakeVisible(deEsserGroup);
    addAndMakeVisible(eqGroup);
    addAndMakeVisible(exciterGroup);


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

    // heigth 600 width 1000

    auto bounds = getLocalBounds().reduced(10);

    // Jede Gruppe bekommt ca. ein Viertel des Fensters in der Höhe
    auto groupHeight = bounds.getHeight() / 4;

    auto compressorArea = bounds.removeFromTop(groupHeight).reduced(10);
    auto deEsserArea    = bounds.removeFromTop(groupHeight).reduced(10);
    auto eqArea         = bounds.removeFromTop(groupHeight).reduced(10);
    auto exciterArea    = bounds.removeFromTop(groupHeight).reduced(10);

    compressorGroup.setBounds(compressorArea.expanded(10));
    deEsserGroup.setBounds(deEsserArea.expanded(10));
    eqGroup.setBounds(eqArea.expanded(10));
    exciterGroup.setBounds(exciterArea.expanded(10));

    // === Compressor Sliders ===
    {
        auto row = compressorArea;
        auto width = row.getWidth() / 4;
        compThresholdSlider.setBounds(row.removeFromLeft(width));
        compRatioSlider.setBounds(row.removeFromLeft(width));
        compAttackSlider.setBounds(row.removeFromLeft(width));
        compReleaseSlider.setBounds(row.removeFromLeft(width));
    }

    // === De-Esser Sliders ===
    {
        auto row = deEsserArea;
        auto width = row.getWidth() / 2;
        deEsserThresholdSlider.setBounds(row.removeFromLeft(width));
        deEsserFreqSlider.setBounds(row.removeFromLeft(width));
    }

    // === EQ Sliders ===
    {
        auto row = eqArea;
        auto width = row.getWidth() / 3;
        eqLowGainSlider.setBounds(row.removeFromLeft(width));
        eqMidGainSlider.setBounds(row.removeFromLeft(width));
        eqHighGainSlider.setBounds(row.removeFromLeft(width));
    }

    // === Exciter Sliders ===
    {
        auto row = exciterArea;
        auto width = row.getWidth() / 2;
        exciterIntensitySlider.setBounds(row.removeFromLeft(width));
        exciterMixSlider.setBounds(row.removeFromLeft(width));
    }

    // === Compressor Sliders + Labels ===
    {
        auto row = compressorArea;
        auto width = row.getWidth() / 4;

        auto s1 = row.removeFromLeft(width);
        compThresholdLabel.setBounds(s1.withHeight(20));
        compThresholdSlider.setBounds(s1.withTrimmedTop(25));

        auto s2 = row.removeFromLeft(width);
        compRatioLabel.setBounds(s2.withHeight(20));
        compRatioSlider.setBounds(s2.withTrimmedTop(25));

        auto s3 = row.removeFromLeft(width);
        compAttackLabel.setBounds(s3.withHeight(20));
        compAttackSlider.setBounds(s3.withTrimmedTop(25));

        auto s4 = row.removeFromLeft(width);
        compReleaseLabel.setBounds(s4.withHeight(20));
        compReleaseSlider.setBounds(s4.withTrimmedTop(25));
    }

    {
        auto row = deEsserArea;
        auto width = row.getWidth() / 2;

        auto s1 = row.removeFromLeft(width);
        deEsserThresholdLabel.setBounds(s1.withHeight(20));
        deEsserThresholdSlider.setBounds(s1.withTrimmedTop(25));

        auto s2 = row.removeFromLeft(width);
        deEsserFreqLabel.setBounds(s2.withHeight(20));
        deEsserFreqSlider.setBounds(s2.withTrimmedTop(25));
    }

    {
        auto row = eqArea;
        auto width = row.getWidth() / 3;

        auto s1 = row.removeFromLeft(width);
        eqLowGainLabel.setBounds(s1.withHeight(20));
        eqLowGainSlider.setBounds(s1.withTrimmedTop(25));

        auto s2 = row.removeFromLeft(width);
        eqMidGainLabel.setBounds(s2.withHeight(20));
        eqMidGainSlider.setBounds(s2.withTrimmedTop(25));

        auto s3 = row.removeFromLeft(width);
        eqHighGainLabel.setBounds(s3.withHeight(20));
        eqHighGainSlider.setBounds(s3.withTrimmedTop(25));
    }

    {
        auto row = exciterArea;
        auto width = row.getWidth() / 2;

        auto s1 = row.removeFromLeft(width);
        exciterIntensityLabel.setBounds(s1.withHeight(20));
        exciterIntensitySlider.setBounds(s1.withTrimmedTop(25));

        auto s2 = row.removeFromLeft(width);
        exciterMixLabel.setBounds(s2.withHeight(20));
        exciterMixSlider.setBounds(s2.withTrimmedTop(25));
    }


    //TODO: remove code below
    /*auto bounds = getLocalBounds().reduced(10);
    auto top = bounds.removeFromTop(150);
    auto middle = bounds.removeFromTop(120);
    auto bottom = bounds.removeFromTop(90);
    auto fotter = bounds.removeFromTop(60);

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
    waveformDisplay.setBounds(bounds.removeFromLeft(40));

    layoutRow(top, compThresholdSlider, compRatioSlider, compAttackSlider, compReleaseSlider);
    layoutRow(middle, deEsserThresholdSlider, deEsserFreqSlider, eqLowGainSlider, eqMidGainSlider);
    layoutRow(bottom, eqHighGainSlider, exciterIntensitySlider, exciterMixSlider, exciterMixSlider);
    layoutRow(fotter,playButton,stopButton,loadFileButton,playButton);// letzter doppelt, falls leer*/
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
            waveformDisplay.setAudioBuffer(processorRef.getLoadedBuffer());
        }
    });


}

void VocalEnhancerEditor::timerCallback()
{
    if (processorRef.isFileLoaded())
    {
        waveformDisplay.setPlayheadPosition(processorRef.getPlayPosition());
    }
}

