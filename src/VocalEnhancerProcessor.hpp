#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "DSP/Exciter_VE.hpp"
#include "DSP/DeEsser_VE.hpp"
#include "DSP/Compressor_VE.hpp"
#include "DSP/Equalizer_VE.hpp"

/**
 * @class VocalEnhancerProcessor
 * @brief AudioProcessor-Klasse zur Sprachbearbeitung mit Equalizer, De-Esser, Exciter und Kompressor.
 *
 * Diese Klasse verwaltet eine vollständige DSP-Kette zur Verbesserung von Sprachsignalen,
 * unterstützt Standalone-Wiedergabe, Parameterverwaltung, Dateiladen und Preset-Funktionen.
 */
class VocalEnhancerProcessor final : public juce::AudioProcessor
{
public:
    /** @brief Konstruktor. */
    VocalEnhancerProcessor();

    /** @brief Destruktor. */
    ~VocalEnhancerProcessor() override = default;

    /**
     * @brief Initialisiert das Plugin vor der Wiedergabe.
     * @param sampleRate Die verwendete Sample-Rate.
     * @param samplesPerBlock Die Blockgröße für die Verarbeitung.
     */
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    /** @brief Gibt nicht mehr benötigte Ressourcen frei (nicht verwendet). */
    void releaseResources() override {}

    /**
     * @brief Führt die Audioblockverarbeitung durch.
     * @param buffer Audiopuffer mit zu bearbeitenden Samples.
     * @param midiMessages MIDI-Puffer (nicht verwendet).
     */
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    /** @brief Erstellt die grafische Benutzeroberfläche. */
    juce::AudioProcessorEditor* createEditor() override;

    /** @brief Gibt an, ob das Plugin eine GUI besitzt. */
    bool hasEditor() const override { return true; }

    /** @brief Gibt den Namen des Plugins zurück. */
    const juce::String getName() const override { return "VoiceEnhancer"; }

    /// MIDI-Funktionalität
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    /**
     * @brief Gibt die Nachklangzeit (Tail Length) zurück.
     * @return Dauer in Sekunden (hier: 0.0).
     */
    double getTailLengthSeconds() const override { return 0.0; }

    /// Programmverwaltung (einfach gehalten)
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    /**
     * @brief Serialisiert den aktuellen Zustand in einen MemoryBlock.
     * @param destMemoryBlock Zielpuffer zur Speicherung.
     */
    void getStateInformation(juce::MemoryBlock& destMemoryBlock) override;

    /**
     * @brief Lädt den Plugin-Zustand aus einem MemoryBlock.
     * @param data Zeiger auf die Daten.
     * @param sizeInBytes Größe der Daten in Bytes.
     */
    void setStateInformation(const void* data, int sizeInBytes) override;

    /** @brief JUCE-integrierte Parameterstruktur. */
    juce::AudioProcessorValueTreeState parameters;

    /**
     * @brief Lädt eine Audiodatei in den internen Puffer.
     * @param audioFile Datei, die geladen werden soll.
     */
    void loadFile(const juce::File& audioFile);

    /**
     * @brief Gibt den geladenen Audiopuffer zurück.
     * @return Referenz auf den geladenen Buffer.
     */
    juce::AudioBuffer<float>& getLoadedBuffer() { return loadedBuffer; }

    /**
     * @brief Gibt zurück, ob eine Datei erfolgreich geladen wurde.
     * @return true, wenn eine Datei geladen ist.
     */
    bool isFileLoaded() const { return fileLoaded; }

    /** @brief Startet die Wiedergabe (nur im Standalone-Modus). */
    void startPlayback();

    /** @brief Stoppt die Wiedergabe (nur im Standalone-Modus). */
    void stopPlayback();

    /**
     * @brief Gibt die aktuelle Wiedergabeposition zurück.
     * @return Position in Sekunden.
     */
    float getPlayHeadPosition() const { return playHeadPositon; }

    /**
     * @brief Gibt an, ob die App im Standalone-Modus läuft.
     * @return true, wenn Standalone.
     */
    bool getIsStandalone() const { return isStandalone; }

    /**
     * @brief Gibt den aktuellen Schreibpuffer zurück.
     * @return Kopie des Schreibpuffers.
     */
    juce::AudioBuffer<float> getWriteBuffer() { return writebuffer; }

    /**
     * @brief Gibt den Puffer zur Darstellung der Wellenform zurück.
     * @return Konstante Referenz auf den Wellenform-Puffer.
     */
    const juce::AudioBuffer<float>& getWaveBuffer() const { return waveBuffer; }

    /** @brief Initialisiert den Schreibpuffer zur Aufnahme. */
    void prepareWriteBuffer();

    /**
     * @brief Gibt die Sample-Rate der geladenen Datei zurück.
     * @return Sample-Rate in Hz.
     */
    double getLoadedSampleRate() const { return loadedSampleRate; }

    /**
     * @brief Repräsentiert den aktuellen Pegel (z. B. zur Visualisierung).
     */
    std::atomic<float> currentLevel = 0.0f;

    /**
     * @brief Lädt Plugin-Parameter aus einer XML-Datei.
     * @param xml Einzigartiger Zeiger auf das XML-Element.
     */
    void loadParametersformXML(std::unique_ptr<juce::XmlElement> xml);

    /**
     * @brief Gibt den aktuellen Parameterbaum zurück.
     * @return Kopie des aktuellen ValueTrees.
     */
    juce::ValueTree getParameters() { return parameters.copyState(); }

    /**
     * @brief Aktiviert oder deaktiviert den De-Esser.
     * @param shouldBypass true = bypassed.
     */
    void setDesserBypass(bool shouldBypass) { deEsser.setBypassed(shouldBypass); }

    /**
     * @brief Aktiviert oder deaktiviert den Equalizer.
     * @param shouldBypass true = bypassed.
     */
    void setEqualizerBypass(bool shouldBypass) { equalizer.setBypass(shouldBypass); }

    /**
     * @brief Aktiviert oder deaktiviert den Exciter.
     * @param shouldBypass true = bypassed.
     */
    void setExciterBypass(bool shouldBypass) { exciter.setBypass(shouldBypass); }

    /**
     * @brief Aktiviert oder deaktiviert den Kompressor.
     * @param shouldBypass true = bypassed.
     */
    void setCompressorBypass(bool shouldBypass) { compressor.setBypassed(shouldBypass); }

private:
    // === DSP-Module ===
    Equalizer_VE equalizer;    ///< Equalizer zur Klangformung.
    Exciter_VE exciter;        ///< Exciter zur Anreicherung von Obertönen.
    Compressor_VE compressor;  ///< Kompressor zur Dynamikbearbeitung.
    DeEsser_VE deEsser;        ///< De-Esser zur Zischlaut-Reduktion.

    // === Datei-Handling ===
    juce::AudioFormatManager formatManager;   ///< JUCE-Formatmanager.
    juce::AudioBuffer<float> loadedBuffer;    ///< Interner Buffer für geladene Dateien.
    bool fileLoaded = false;                  ///< Datei erfolgreich geladen?

    // === Wiedergabe ===
    int playPosition = 0;                     ///< Aktuelle Abspielposition in Samples.
    float playHeadPositon = 0;                ///< Aktuelle Wiedergabeposition.
    bool isPlaying = false;                   ///< Wiedergabestatus.
    bool isStandalone = juce::JUCEApplicationBase::isStandaloneApp(); ///< Standalone-Modus aktiv?

    // === Buffer-Verwaltung ===
    juce::AudioBuffer<float> writebuffer;     ///< Buffer für Write/Recording.
    double loadedSampleRate = 44100.0;        ///< Samplerate der geladenen Datei.

    // === Dynamik ===
    juce::ADSR adsr;                          ///< ADSR-Hüllkurve für Fades.
    juce::ADSR::Parameters adsrParams;        ///< Parameter der Hüllkurve.

    bool hasCopiedFromHost = false;           ///< Statusflag für Bufferübernahme.
    juce::AudioBuffer<float> waveBuffer;      ///< Buffer für Wellenformanzeige.
    bool wasPlaying = false;                  ///< Vorheriger Abspielstatus.

    /**
     * @brief Interne DSP-Verarbeitungskette.
     * @param buffer Audiopuffer.
     */
    void dspChain(juce::AudioBuffer<float>& buffer);

    /** @brief Lädt die Standard-Parameterwerte. */
    void loadParameters();

    /**
     * @brief Erstellt das Parameterlayout für die GUI und Automatisierung.
     * @return JUCE-ParameterLayout-Objekt.
     */
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalEnhancerProcessor)
};
