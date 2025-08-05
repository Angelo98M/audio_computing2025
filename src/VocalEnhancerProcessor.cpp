#include "VocalEnhancerProcessor.hpp"
#include "VocalEnhancerEditor.hpp"

VocalEnhancerProcessor::VocalEnhancerProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                       .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    formatManager.registerBasicFormats();
}

void VocalEnhancerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const auto numChannels = getTotalNumOutputChannels();
    loadedSampleRate = sampleRate;

    adsr.setSampleRate(loadedSampleRate);
    equalizer.prepare(loadedSampleRate, samplesPerBlock, numChannels);
    compressor.prepare(loadedSampleRate, samplesPerBlock, numChannels);
    deEsser.prepare(loadedSampleRate, samplesPerBlock, numChannels);
    exciter.prepare(loadedSampleRate, samplesPerBlock, numChannels);

}

void VocalEnhancerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {

    juce::ScopedNoDenormals noDenormals;

    const int numSamplesToProcess = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (isStandalone) {
        // === Datei-Playback ===
        if (fileLoaded && isPlaying)
        {
            // Sicherstellen, dass wir nicht über das Ende hinaus lesen
            const int remainingSamples = loadedBuffer.getNumSamples() - playPosition;
            const int samplesToCopy = juce::jmin(numSamplesToProcess, remainingSamples);

            for (int ch = 0; ch < numChannels; ++ch)
            {
                if (ch < loadedBuffer.getNumChannels())
                {
                    buffer.copyFrom(ch, 0, loadedBuffer, ch, playPosition, samplesToCopy);
                }
                else
                {
                    buffer.clear(ch, 0, numSamplesToProcess); // falls zu viele Output-Kanäle
                }
            }

            // Rest mit Nullen auffüllen, falls Datei kürzer als Buffer
            if (samplesToCopy < numSamplesToProcess)
            {
                for (int ch = 0; ch < numChannels; ++ch)
                    buffer.clear(ch, samplesToCopy, numSamplesToProcess - samplesToCopy);
            }

            playPosition += samplesToCopy;

            if (playPosition >= loadedBuffer.getNumSamples())
            {
                stopPlayback(); // Oder playPosition = 0; // für Looping
            }
        }
        else
        {
            // Falls keine Datei oder Wiedergabe gestoppt
            buffer.clear();
        }
    }
    else {
        juce::AudioPlayHead::CurrentPositionInfo posInfo;
        if (auto* playHead = getPlayHead())
        {
            if (playHead->getCurrentPosition(posInfo))
            {
                if (posInfo.isPlaying)
                {
                    // Reaper spielt → ADSR starten
                    if (!wasPlaying)
                    {
                        adsr.noteOn();
                        wasPlaying = true;
                    }
                }
                else
                {
                    // Reaper gestoppt → ADSR stoppen
                    if (wasPlaying)
                    {
                        adsr.noteOff();
                        wasPlaying = false;
                    }
                }
            }
        }
        if (buffer.getNumSamples() > 0)
        {
            waveBuffer.makeCopyOf(buffer);

        }
    }

    loadParameters();

    if (numSamplesToProcess > 0)
    {

        // ADSR
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* channelData = buffer.getWritePointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                channelData[i] *= adsr.getNextSample();
            }
        }

        dspChain(buffer);

        //get Volum
        float maxLevel = 0.0f;
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getReadPointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                maxLevel = std::max(maxLevel, std::abs(data[i]));
            }
        }

        currentLevel.store(maxLevel); // atomar speichern

    }

}

void VocalEnhancerProcessor::prepareWriteBuffer()
{
    if (!fileLoaded || loadedBuffer.getNumSamples() == 0)
        return;

    jassert(loadedBuffer.getNumSamples() > 0);
    jassert(loadedBuffer.getNumChannels() > 0);
    writebuffer.makeCopyOf(loadedBuffer);
    std::cout << "writebuffer size: " << writebuffer.getNumChannels() << std::endl;
    std::cout<< writebuffer.getNumSamples() << " samples";

    int blockSize = writebuffer.getNumSamples(); // gesamter Buffer

    compressor.prepare(loadedSampleRate, blockSize, writebuffer.getNumChannels());
    deEsser.prepare(loadedSampleRate, blockSize, writebuffer.getNumChannels());
    equalizer.prepare(loadedSampleRate, blockSize, writebuffer.getNumChannels());
    exciter.prepare(loadedSampleRate, blockSize, writebuffer.getNumChannels());

    // Parameter wie in processBlock holen
    compressor.setThreshold(parameters.getRawParameterValue("compThreshold")->load());
    compressor.setRatio(parameters.getRawParameterValue("compRatio")->load());
    compressor.setAttack(parameters.getRawParameterValue("compAttack")->load());
    compressor.setRelease(parameters.getRawParameterValue("compRelease")->load());

    deEsser.setThreshold(parameters.getRawParameterValue("deessThreshold")->load());
    deEsser.setFrequency(parameters.getRawParameterValue("deessFreq")->load());

    equalizer.updateFilters(
        parameters.getRawParameterValue("eqLowPassFreq")->load(),
        100.0f,
        parameters.getRawParameterValue("eqLowGain")->load(), 0.7f,
        1000.0f,
        parameters.getRawParameterValue("eqMidGain")->load(), 1.0f,
        8000.0f,
        parameters.getRawParameterValue("eqHighGain")->load(), 0.7f,
        parameters.getRawParameterValue("eqHighPassFreq")->load()
    );

    exciter.setIntensity(parameters.getRawParameterValue("exciterIntensity")->load());
    exciter.setMix(parameters.getRawParameterValue("exciterMix")->load());

    // Effekte auf gesamten Buffer anwenden
    compressor.processBlock(writebuffer);
    deEsser.processBlock(writebuffer);
    equalizer.processBlock(writebuffer);
    exciter.processBlock(writebuffer);
}


juce::AudioProcessorEditor* VocalEnhancerProcessor::createEditor()
{
    return new VocalEnhancerEditor(*this);
}

void VocalEnhancerProcessor::dspChain(juce::AudioBuffer<float>& buffer) {
    compressor.processBlock(buffer);
    deEsser.processBlock(buffer);
    equalizer.processBlock(buffer);
    exciter.processBlock(buffer);
}

void VocalEnhancerProcessor::loadParameters() {
    // Parameter abrufen
    compressor.setThreshold(parameters.getRawParameterValue("compThreshold")->load());
    compressor.setRatio(parameters.getRawParameterValue("compRatio")->load());
    compressor.setAttack(parameters.getRawParameterValue("compAttack")->load());
    compressor.setRelease(parameters.getRawParameterValue("compRelease")->load());

    deEsser.setThreshold(parameters.getRawParameterValue("deessThreshold")->load());
    deEsser.setFrequency(parameters.getRawParameterValue("deessFreq")->load());

    adsrParams.attack  = parameters.getRawParameterValue("adsrAttack")->load() / 1000.0f;
    adsrParams.decay   = parameters.getRawParameterValue("adsrDecay")->load() / 1000.0f;
    adsrParams.sustain = parameters.getRawParameterValue("adsrSustain")->load();
    adsrParams.release = parameters.getRawParameterValue("adsrRelease")->load() / 1000.0f;

    equalizer.updateFilters(
        parameters.getRawParameterValue("eqLowPassFreq")->load(),
        100.0f,
        parameters.getRawParameterValue("eqLowGain")->load(), 0.7f,
        1000.0f,
        parameters.getRawParameterValue("eqMidGain")->load(), 1.0f,
        8000.0f,
        parameters.getRawParameterValue("eqHighGain")->load(), 0.7f,
        parameters.getRawParameterValue("eqHighPassFreq")->load()
    );

    exciter.setIntensity(parameters.getRawParameterValue("exciterIntensity")->load());
    exciter.setMix(parameters.getRawParameterValue("exciterMix")->load());

    adsr.setParameters(adsrParams);
}

juce::AudioProcessorValueTreeState::ParameterLayout VocalEnhancerProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // === Compressor ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compThreshold", "Comp Threshold", -60.0f, 0.0f, -24.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compRatio",     "Comp Ratio",     1.0f, 20.0f, 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compAttack",    "Comp Attack",    0.1f, 100.0f, 10.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("compRelease",   "Comp Release",   5.0f, 300.0f, 100.0f));

    // === De-Esser ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>("deessThreshold", "De-Esser Threshold", -60.0f, 0.0f, -30.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("deessFreq",      "De-Esser Freq",      2000.0f, 12000.0f, 6000.0f));

    // === Equalizer ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqLowGain",  "EQ Low Gain",  -24.0f, 24.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqMidGain",  "EQ Mid Gain",  -24.0f, 24.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqHighGain", "EQ High Gain", -24.0f, 24.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqLowPassFreq", "EQ LowPassFreq",juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),20000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("eqHighPassFreq", "EQ HighPassFreq",juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f),20.0f));

    // === Exciter ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>("exciterIntensity", "Exciter Intensity", 0.0f, 1.0f, 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("exciterMix",       "Exciter Mix",       0.0f, 1.0f, 0.5f));

    // === ADSR ===
    params.push_back(std::make_unique<juce::AudioParameterFloat>("adsrAttack",  "ADSR Attack",  1.0f, 5000.0f, 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("adsrDecay",   "ADSR Decay",   1.0f, 5000.0f, 100.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("adsrSustain", "ADSR Sustain", 0.0f, 1.0f,    0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("adsrRelease", "ADSR Release", 1.0f, 5000.0f, 300.0f));


    return { params.begin(), params.end() };
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalEnhancerProcessor();
}

void VocalEnhancerProcessor::loadFile(const juce::File& audioFile) {
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(audioFile));

    if (reader != nullptr)
    {
        const int numChannels = static_cast<int>(reader->numChannels);
        const int numSamples = static_cast<int>(reader->lengthInSamples);

        loadedBuffer.setSize(numChannels, numSamples);
        reader->read(&loadedBuffer, 0, numSamples, 0, true, true);

        fileLoaded = true;

        juce::Logger::writeToLog("File loaded: " + audioFile.getFileName());
        juce::Logger::writeToLog("Channels: " + juce::String(numChannels) + ", Samples: " + juce::String(numSamples));
    }
    else
    {
        juce::Logger::writeToLog("Failed to load file: " + audioFile.getFullPathName());
        fileLoaded = false;
    }
}

void VocalEnhancerProcessor::loadParametersformXML(std::unique_ptr<juce::XmlElement> xml) {
    if (xml && xml->hasTagName(parameters.state.getType()))
    {
        juce::ValueTree tree = juce::ValueTree::fromXml(*xml);
        parameters.replaceState(tree);
    }
}


void VocalEnhancerProcessor::startPlayback()
{
    playPosition = 0;
    isPlaying = true;
    adsr.noteOn();
}

void VocalEnhancerProcessor::stopPlayback()
{
    isPlaying = false;
    adsr.noteOff();
}

void VocalEnhancerProcessor::getStateInformation(juce::MemoryBlock& destData) {
}

void VocalEnhancerProcessor::setStateInformation(const void* data, int sizeInBytes) {
}
