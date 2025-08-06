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
    /**
     * @brief Konstruktor.
     */
    VocalEnhancerProcessor();

    /**
     * @brief Destruktor.
     */
    ~VocalEnhancerProcessor() override = default;

    /**
     * @brief Initialisierung des DSP-Setups.
     * @param sampleRate Die verwendete Samplerate.
     * @param samplesPerBlock Blockgröße der Verarbeitung.
     */
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    /**
     * @brief Gibt unbenötigte Ressourcen frei (nicht verwendet).
     */
    void releaseResources() override {}

    /**
     * @brief Verarbeitet einen Audioblock.
     * @param buffer Der Audiopuffer.
     * @param midiMessages MIDI-Puffer (nicht verwendet).
     */
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    /**
     * @brief Erstellt den GUI-Editor für das Plugin.
     * @return Ein Zeiger auf die Editor-Komponente.
     */
    juce::AudioProcessorEditor* createEditor() override;

    /**
     * @brief Gibt an, ob das Plugin eine Editor-GUI hat.
     */
    bool hasEditor() const override { return true; }

    /**
     * @brief Gibt den Namen des Plugins zurück.
     */
    const juce::String getName() const override { return "VoiceEnhancer"; }

    /// MIDI-Unterstützung
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    /**
     * @brief Gibt die Nachklangdauer des Effekts zurück.
     * @return Dauer in Sekunden (hier 0.0).
     */
    double getTailLengthSeconds() const override { return 0.0; }

    /// Programmverwaltung (einfach gehalten)
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    /**
     * @brief Serialisiert den Zustand in einen MemoryBlock.
     * @param destMemoryBlock Ziel-MemoryBlock.
     */
    void getStateInformation(juce::MemoryBlock& destMemoryBlock) override;

    /**
     * @brief Lädt Plugin-Zustand aus einem MemoryBlock.
     * @param data Zeiger auf die Daten.
     * @param sizeInBytes Größe der Daten in Bytes.
     */
    void setStateInformation(const void* data, int sizeInBytes) override;

    /**
     * @brief Der zentrale Parameterbaum (automatisiert mit JUCE).
     */
    juce::AudioProcessorValueTreeState parameters;

    /**
     * @brief Lädt eine Audiodatei in den Arbeitspuffer.
     * @param audioFile Die zu ladende Datei.
     */
    void loadFile(const juce::File& audioFile);

    /**
     * @brief Gibt Referenz auf den geladenen Audiopuffer zurück.
     */
    juce::AudioBuffer<float>& getLoadedBuffer() { return loadedBuffer; }

    /**
     * @brief Gibt an, ob eine Datei erfolgreich geladen wurde.
     */
    bool isFileLoaded() const { return fileLoaded; }

    /**
     * @brief Startet die Wiedergabe (nur im Standalone-Modus).
     */
    void startPlayback();

    /**
     * @brief Stoppt die Wiedergabe (nur im Standalone-Modus).
     */
    void stopPlayback();

    /**
     * @brief Gibt die aktuelle Position des Wiedergabekopfs zurück.
     * @return Position in Sekunden.
     */
    float getPlayHeadPosition() const { return playHeadPositon; }

    /**
     * @brief Gibt an, ob die App im Standalone-Modus läuft.
     */
    bool getIsStandalone() const { return isStandalone; }

    /**
     * @brief Gibt den aktuellen Schreibpuffer zurück.
     * @return Eine Kopie des Write-Buffers.
     */
    juce::AudioBuffer<float> getWriteBuffer() { return writebuffer; }

    /**
     * @brief Gibt den Puffer zur Wellenformdarstellung zurück.
     * @return Konstante Referenz auf den Wave-Buffer.
     */
    const juce::AudioBuffer<float>& getWaveBuffer() const { return waveBuffer; }

    /**
     * @brief Initialisiert den Schreibpuffer zur Aufnahme.
     */
    void prepareWriteBuffer();

    /**
     * @brief Gibt die Samplerate der geladenen Datei zurück.
     * @return Sample Rate in Hz.
     */
    double getLoadedSampleRate() const { return loadedSampleRate; }

    /**
     * @brief Repräsentiert den aktuellen RMS-Level (wird extern gelesen).
     */
    std::atomic<float> currentLevel = 0.0f;

    /**
     * @brief Lädt Parameter aus einer XML-Struktur.
     * @param xml Pointer auf XML-Element mit den Parametern.
     */
    void loadParametersformXML(std::unique_ptr<juce::XmlElement> xml);

    /**
     * @brief Gibt den aktuellen Parameter-Tree zurück (Kopie).
     */
    juce::ValueTree getParameters() { return parameters.copyState(); }

private:
    // === DSP-Module ===
    Equalizer_VE equalizer;   ///< Equalizer zur Klangformung
    Exciter_VE exciter;       ///< Exciter für Obertöne
    Compressor_VE compressor; ///< Dynamikbearbeitung
    DeEsser_VE deEsser;       ///< Zischlaut-Reduktion

    // === Datei-Handling ===
    juce::AudioFormatManager formatManager; ///< JUCE Formatmanager
    juce::AudioBuffer<float> loadedBuffer;  ///< Audio-Datenpuffer
    bool fileLoaded = false;                ///< Status der Dateiladung

    // === Wiedergabe ===
    int playPosition = 0;           ///< Aktuelle Abspielposition in Samples
    float playHeadPositon = 0;      ///< Position des Wiedergabekopfs
    bool isPlaying = false;         ///< Wiedergabestatus
    bool isStandalone = juce::JUCEApplicationBase::isStandaloneApp(); ///< Standalone-Flag

    // === Buffer-Verwaltung ===
    juce::AudioBuffer<float> writebuffer; ///< Schreibpuffer
    double loadedSampleRate = 44100.0;    ///< Samplerate der geladenen Datei

    // === Dynamik (z. B. Fade In/Out) ===
    juce::ADSR adsr;                      ///< ADSR-Hüllkurve
    juce::ADSR::Parameters adsrParams;    ///< ADSR-Parameter

    bool hasCopiedFromHost = false;       ///< Status für Hostübernahme
    juce::AudioBuffer<float> waveBuffer;  ///< Puffer für Wellenformanzeige
    bool wasPlaying = false;              ///< Wiedergabezustand davor

    /**
     * @brief Interne Verarbeitungskette für DSP-Module.
     * @param buffer Der Audiopuffer.
     */
    void dspChain(juce::AudioBuffer<float>& buffer);

    /**
     * @brief Lädt die Default-Parameterwerte.
     */
    void loadParameters();

    /**
     * @brief Erstellt das JUCE-Parameterlayout für den Parameterbaum.
     * @return Das Parameterlayout.
     */
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalEnhancerProcessor)
};
