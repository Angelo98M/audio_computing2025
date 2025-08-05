#include "VocalEnhancerEditor.hpp"

VocalEnhancerEditor::VocalEnhancerEditor(VocalEnhancerProcessor& p)
    : AudioProcessorEditor(p), processorRef(p),levelMeter(p.currentLevel)
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
    configureSlider(eqLowPassSlider);
    configureSlider(eqHighPassSlider);

    eqLowGainAttachment  = std::make_unique<SliderAttachment>(processorRef.parameters, "eqLowGain", eqLowGainSlider);
    eqMidGainAttachment  = std::make_unique<SliderAttachment>(processorRef.parameters, "eqMidGain", eqMidGainSlider);
    eqHighGainAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "eqHighGain", eqHighGainSlider);
    eqLowPassAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "eqLowPassFreq", eqLowPassSlider);
    eqHighPassAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "eqHighPassFreq", eqHighPassSlider);

    // === Exciter ===
    configureSlider(exciterIntensitySlider);
    configureSlider(exciterMixSlider);

    exciterIntensityAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "exciterIntensity", exciterIntensitySlider);
    exciterMixAttachment       = std::make_unique<SliderAttachment>(processorRef.parameters, "exciterMix", exciterMixSlider);

    //ADSR

    configureSlider(adsrAttackSlider);
    configureSlider(adsrDecaySlider);
    configureSlider(adsrSustainSlider);
    configureSlider(adsrReleaseSlider);

    adsrAttackAttachment  = std::make_unique<SliderAttachment>(processorRef.parameters, "adsrAttack",  adsrAttackSlider);
    adsrDecayAttachment   = std::make_unique<SliderAttachment>(processorRef.parameters, "adsrDecay",   adsrDecaySlider);
    adsrSustainAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "adsrSustain", adsrSustainSlider);
    adsrReleaseAttachment = std::make_unique<SliderAttachment>(processorRef.parameters, "adsrRelease", adsrReleaseSlider);


    // === File Browser ===

    loadFileButton.onClick = [this]() { openFileChooser(); };

    saveButton.onClick = [this] {exportProcessedFile();};

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
        &eqLowGainSlider, &eqMidGainSlider, &eqHighGainSlider,&eqLowPassSlider,&eqHighPassSlider,
        &exciterIntensitySlider, &exciterMixSlider,
        &adsrAttackSlider, &adsrDecaySlider, &adsrSustainSlider,&adsrReleaseSlider
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
    setupLabel(eqLowPassLabel, "Low Pass");
    setupLabel(eqHighPassLabel, "High Pass");

    // === Exciter Labels ===
    setupLabel(exciterIntensityLabel, "Intensity");
    setupLabel(exciterMixLabel, "Mix");

    // === ADSR Labels ===
    setupLabel(adsrAttackLabel, "Attack");
    setupLabel(adsrDecayLabel, "Decay");
    setupLabel(adsrSustainLabel, "Sustain");
    setupLabel(adsrReleaseLabel, "Release");

    addAndMakeVisible(waveformDisplay);

    for (auto* s : sliders)
        addAndMakeVisible(*s);

    // === Group Components anzeigen ===
    addAndMakeVisible(compressorGroup);
    addAndMakeVisible(deEsserGroup);
    addAndMakeVisible(eqGroup);
    addAndMakeVisible(exciterGroup);
    addAndMakeVisible(adsrGroup);

    // ComboBox + Button sichtbar machen
    addAndMakeVisible(profileComboBox);
    addAndMakeVisible(reloadProfilesButton);


    // Erstinitialisierung mit aktuellen Werten
    waveformDisplay.setADSR(
        processorRef.parameters.getRawParameterValue("adsrAttack")->load(),
        processorRef.parameters.getRawParameterValue("adsrDecay")->load(),
        processorRef.parameters.getRawParameterValue("adsrSustain")->load(),
        processorRef.parameters.getRawParameterValue("adsrRelease")->load()
    );

    adsrAttackSlider.onValueChange = [this]() {
        updateADSRVisual();
    };
    adsrDecaySlider.onValueChange = [this]() {
        updateADSRVisual();
    };
    adsrSustainSlider.onValueChange = [this]() {
        updateADSRVisual();
    };
    adsrReleaseSlider.onValueChange = [this]() {
        updateADSRVisual();
    };

    // Items laden
    reloadProfileList();

    // Profilwechsel behandeln
    profileComboBox.onChange = [this]() {
        auto selectedName = profileComboBox.getText();
        if (selectedName.isNotEmpty())
            loadProfileFromName(selectedName);
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
                    saveProfileWithName(profileName);
                    reloadProfileList(); // ComboBox neu füllen
                }
            }

            delete window; // Fenster freigeben
        }));
    };

    if (processorRef.getIsStandalone()) {
        //addAndMakeVisible(saveButton);
        // === Play ===
        addAndMakeVisible(playButton);
        addAndMakeVisible(stopButton);
        addAndMakeVisible(loadFileButton);
    }


    addAndMakeVisible(levelMeter);

    setSize(755, 750);
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
        auto width = row.getWidth() / 5;

        auto s1 = row.removeFromLeft(width);
        eqLowGainLabel.setBounds(s1.withHeight(20));
        eqLowGainSlider.setBounds(s1.withTrimmedTop(25));

        auto s2 = row.removeFromLeft(width);
        eqMidGainLabel.setBounds(s2.withHeight(20));
        eqMidGainSlider.setBounds(s2.withTrimmedTop(25));

        auto s3 = row.removeFromLeft(width);
        eqHighGainLabel.setBounds(s3.withHeight(20));
        eqHighGainSlider.setBounds(s3.withTrimmedTop(25));

        auto s4 = row.removeFromLeft(width);
        eqLowPassLabel.setBounds(s4.withHeight(20));
        eqLowPassSlider.setBounds(s4.withTrimmedTop(25));

        auto s5 = row.removeFromLeft(width);
        eqHighPassLabel.setBounds(s5.withHeight(20));
        eqHighPassSlider.setBounds(s5.withTrimmedTop(25));
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

    adsrGroup.setBounds(20, 530, 400, 120);
    auto adsrArea = adsrGroup.getBounds().reduced(10);
    // === Compressor Sliders + Labels ===
    {
        auto row = adsrArea;
        auto width = row.getWidth() / 4;

        auto s1 = row.removeFromLeft(width);
        adsrAttackLabel.setBounds(s1.withHeight(20));
        adsrAttackSlider.setBounds(s1.withTrimmedTop(25));

        auto s2 = row.removeFromLeft(width);
        adsrDecayLabel.setBounds(s2.withHeight(20));
        adsrDecaySlider.setBounds(s2.withTrimmedTop(25));

        auto s3 = row.removeFromLeft(width);
        adsrSustainLabel.setBounds(s3.withHeight(20));
        adsrSustainSlider.setBounds(s3.withTrimmedTop(25));

        auto s4 = row.removeFromLeft(width);
        adsrReleaseLabel.setBounds(s4.withHeight(20));
        adsrReleaseSlider.setBounds(s4.withTrimmedTop(25));
    }

    playButton.setBounds(410,690,50,50);
    stopButton.setBounds(470,690,50,50);
    loadFileButton.setBounds(640,690,100,50);
    saveButton.setBounds(530,690,100,50);
    waveformDisplay.setBounds(10,10,740,100);
    profileComboBox.setBounds(20, 690, 200, 24);
    reloadProfilesButton.setBounds(230, 690, 30, 24);
    saveProfileButton.setBounds(270, 690, 120, 24);
    levelMeter.setBounds(500, 530, 50, 150); // Rechts am Rand



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

    auto profiles = getAvailableProfiles();
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
    if (!processorRef.getIsStandalone()) {
        if (processorRef.getWaveBuffer().getNumSamples() > 0)
        {
            waveformDisplay.setAudioBuffer(processorRef.getWaveBuffer());
            waveformDisplay.setPlayheadPosition(processorRef.getPlayPosition());
        }


    }
    else {
        if (processorRef.isFileLoaded())
        {
            waveformDisplay.setPlayheadPosition(processorRef.getPlayPosition());

        }
    }
    waveformDisplay.repaint();
}

void VocalEnhancerEditor::updateADSRVisual()
{
    waveformDisplay.setADSR(
    processorRef.parameters.getRawParameterValue("adsrAttack")->load(),
    processorRef.parameters.getRawParameterValue("adsrDecay")->load(),
    processorRef.parameters.getRawParameterValue("adsrSustain")->load(),
    processorRef.parameters.getRawParameterValue("adsrRelease")->load()
    );
}


juce::File VocalEnhancerEditor::getProfileDirectory() const {
    auto dir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                    .getChildFile("VocalEnhancerProfiles");

    if (!dir.exists())
        dir.createDirectory(); // wird beim ersten Mal angelegt

    return dir;
}

std::vector<juce::File> VocalEnhancerEditor::getAvailableProfiles() const {
    juce::Array<juce::File> files = getProfileDirectory().findChildFiles(juce::File::findFiles, false, "*.profile");

    std::vector<juce::File> profileList;
    for (const auto& file : files)
        profileList.push_back(file);

    return profileList;
}

void VocalEnhancerEditor::saveProfileWithName(const juce::String &profileName) {
    auto file = getProfileDirectory().getChildFile(profileName + ".profile");

    auto state = processorRef.getParameters();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    if (xml)
        xml->writeTo(file);
}

void VocalEnhancerEditor::loadProfileFromName(const juce::String &profileName) {
    auto file = getProfileDirectory().getChildFile(profileName + ".profile");

    if (!file.existsAsFile())
        return;

    processorRef.loadParametersformXML(juce::XmlDocument::parse(file));
}

