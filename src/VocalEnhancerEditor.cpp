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

    saveButton.onClick = [this] {exportProcessedFile();};

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

    // ComboBox + Button sichtbar machen
    addAndMakeVisible(profileComboBox);
    addAndMakeVisible(reloadProfilesButton);

    // Items laden
    reloadProfileList();

    // Profilwechsel behandeln
    profileComboBox.onChange = [this]() {
        auto selectedName = profileComboBox.getText();
        if (selectedName.isNotEmpty())
            processorRef.loadProfileFromName(selectedName);
    };

    // Manuelles Neuladen (falls du neue Dateien einfügst etc.)
    reloadProfilesButton.onClick = [this]() {
        reloadProfileList();
    };

    addAndMakeVisible(saveProfileButton);

    saveProfileButton.onClick = [this]() {
        auto* window = new juce::AlertWindow(
            "Neues Profil speichern",
            "Bitte Profilnamen eingeben:",
            juce::AlertWindow::NoIcon
        );

        window->addTextEditor("profileName", "", "Profilname:");
        window->addButton("OK", 1);
        window->addButton("Abbrechen", 0);

        // `enterModalState()` macht AlertWindow asynchron
        window->enterModalState(true, juce::ModalCallbackFunction::create([this, window](int result) {
            if (result == 1)  // OK gedrückt
            {
                auto profileName = window->getTextEditor("profileName")->getText().trim();

                if (profileName.isNotEmpty())
                {
                    processorRef.saveProfileWithName(profileName);
                    reloadProfileList(); // ComboBox neu füllen
                }
            }

            delete window; // Fenster freigeben
        }));
    };



    // if (processorRef.getIsStandalone()) addAndMakeVisible(saveButton);

    setSize(755, 600);
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

    compressorGroup.setBounds(20, 130, 400, 120);
    deEsserGroup.setBounds(440, 130, 300, 120);
    eqGroup.setBounds(20, 260, 720, 120);
    exciterGroup.setBounds(20, 400, 720, 120);

    auto compressorArea = compressorGroup.getBounds().reduced(10);
    auto deEsserArea    = deEsserGroup.getBounds().reduced(10);
    auto eqArea         = eqGroup.getBounds().reduced(10);
    auto exciterArea    = exciterGroup.getBounds().reduced(10);


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

    playButton.setBounds(410,540,50,50);
    stopButton.setBounds(470,540,50,50);
    loadFileButton.setBounds(640,540,100,50);
    saveButton.setBounds(530,540,100,50);
    waveformDisplay.setBounds(10,10,740,100);
    profileComboBox.setBounds(20, 540, 200, 24);
    reloadProfilesButton.setBounds(230, 540, 30, 24);
    saveProfileButton.setBounds(270, 540, 120, 24);

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

void VocalEnhancerEditor::exportProcessedFile()
{
    saveChooser = std::make_unique<juce::FileChooser>("WAV-Datei speichern...",
                                                        juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
                                                        "*.wav");

    auto chooserFlags = juce::FileBrowserComponent::saveMode |
                        juce::FileBrowserComponent::canSelectFiles;

    saveChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
    {
        auto file = fc.getResult();

        if (file == juce::File{})
        {
            DBG("Speichern abgebrochen.");
            return;
        }

        juce::File finalFile = file.withFileExtension(".wav");

        processorRef.prepareWriteBuffer();
        const auto& buffer = processorRef.getWriteBuffer();

        if (buffer.getNumSamples() == 0)
        {
            DBG("Kein Audio im writeBuffer vorhanden!");
            return;
        }

        juce::WavAudioFormat wavFormat;
        auto outputStream = finalFile.createOutputStream();

        if (!outputStream)
        {
            DBG("Konnte keinen OutputStream erzeugen!");
            return;
        }

        std::unique_ptr<juce::AudioFormatWriter> writer(
            wavFormat.createWriterFor(outputStream.get(),
                                      processorRef.getLoadedSampleRate(),
                                      buffer.getNumChannels(),
                                      16,
                                      {},
                                      0)
        );

        if (!writer)
        {
            DBG("Konnte keinen WAV-Writer erzeugen!");
            return;
        }


        outputStream.release(); // Übergibt ownership an writer

        if (writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples()))
        {

            DBG("Export erfolgreich: " + finalFile.getFullPathName());
        }
        else
        {
            DBG("Export fehlgeschlagen!");
        }
    });
}

void VocalEnhancerEditor::reloadProfileList()
{
    profileComboBox.clear();

    auto profiles = processorRef.getAvailableProfiles();
    int id = 1;
    for (const auto& file : profiles)
    {
        auto name = file.getFileNameWithoutExtension();
        profileComboBox.addItem(name, id++);
    }

    if (profileComboBox.getNumItems() > 0)
        profileComboBox.setSelectedItemIndex(0, juce::dontSendNotification);
}


void VocalEnhancerEditor::timerCallback()
{
    if (processorRef.isFileLoaded())
    {
        waveformDisplay.setPlayheadPosition(processorRef.getPlayPosition());
    }
}

