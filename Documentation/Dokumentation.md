# 🧠 Architektur und Implementierung – Vocal Enhancer

Diese Datei beschreibt die Architektur, Signalverarbeitung, Benutzeroberfläche und weitere technische Details des Audio-Plugins **Simply better Voice** (Vocal Enhancer).

---

## 🏗️ Architekturübersicht

Das Projekt ist modular aufgebaut und folgt der klassischen Trennung von **GUI** und **Signalverarbeitung**. Es basiert auf dem **JUCE Framework** und verwendet **CMake** als Build-System.

### 🔹 Module & Hauptklassen

- `VocalEnhancerProcessor.cpp`: Hauptklasse für die Audioverarbeitung (DSP-Kern)
- `VocalEnhancerEditor.cpp`: Verwaltung und Zeichnung der Benutzeroberfläche
- `WaveformDisplay.cpp`: Visualisierung des Audiosignals
- `DSP/Compressor_VE.cpp`: Sprachspezifischer Kompressor
- `DSP/Equalizer_VE.cpp`: 3-Band-EQ zur Klanganpassung
- `DSP/DeEsser_VE.cpp`: De-Esser zur Entfernung von Zischlauten
- `DSP/Exciter_VE.cpp`: Exciter zur Anhebung hoher Frequenzen und Verbesserung der Sprachverständlichkeit

für nähren einblick [Zur Doxygen-Dokumentation](html/index.html)

---

## 🎛️ Erklärung der Signalverarbeitung (DSP-Komponenten)

Die Signalverarbeitung erfolgt in mehreren hintereinander geschalteten Modulen:

1. **De-Esser:**
   - Erkennt und reduziert sibilante Frequenzen (Zischlaute)
   - Arbeitet frequenzselektiv mit einem Hochpassfilter und Pegelvergleicher

2. **Equalizer (EQ):**
   - Ermöglicht die Korrektur von Tiefen, Mitten und Höhen
   - Verwendet IIR-Filter aus dem JUCE DSP-Modul

3. **Kompressor:**
   - Reduziert die Dynamik des Sprachsignals

4. **Exciter:**
   - Fügt Obertöne hinzu, um das Signal brillanter wirken zu lassen
   - Nutzt ein dynamisches Waveshaping-Verfahren


Alle DSP-Komponenten sind modular, leicht austauschbar und können dynamisch aktiviert/deaktiviert werden.

---

## 🖼️ Beschreibung des GUI-Aufbaus

Die Benutzeroberfläche wurde mithilfe von JUCE-Komponenten erstellt.

### GUI-Komponenten:

- **Slider** für die Steuerung von EQ, Kompressor, Exciter, De-Esser
- **Schalter (ToggleButtons)** zum Aktivieren/Deaktivieren einzelner Module
- **WaveformDisplay** zeigt das aktuelle Audiosignal an (selbst entwickelt/nur Standalone) mit ASDR Visualisirung

Das Layout ist responsiv und erlaubt einfache Erweiterbarkeit.

---

## 🔄 Beschreibung des Signalflusses

```
[Eingang] 
   ↓
[ADSR]
   ↓
[De-Esser]
   ↓
[Equalizer]
   ↓
[Kompressor]
   ↓
[Exciter]
   ↓
[Ausgang]
```

Optional können Module im Signalfluss deaktiviert werden (Bypass).

---

## ✨ Besonderheiten

- 🎨 **Eigenentwickelte GUI-Komponente**: `WaveformDisplay` für Live-Signalvisualisierung und selbstgebaute `LevelMeterComponent` um den Laustärke pegel wiederzugeben
- 🔄 **Modularer DSP-Aufbau**: Erleichtert zukünftige Erweiterungen (z. B. Limiter, Gate)
- 🧪 **Formatflexibel**: Unterstützung für VST3 und Standalone durch JUCE-Plugin-Wrapper

---

## 📚 Externe Libraries

- **JUCE** (https://github.com/juce-framework/JUCE)  
  - Für GUI, Plugin-Wrapping, Audio I/O, DSP-Basisklassen, VST3/AU Unterstützung
- **CMake** (https://cmake.org/)  
  - Build-System zur plattformübergreifenden Generierung von Projektdateien

Weitere externe Bibliotheken werden aktuell nicht verwendet.

---

## 📁 Verzeichnisstruktur (relevant)

```
.
├── CMakeLists.txt
├── extern/
│   └── JUCE/                # JUCE Submodul
├── src/
│   ├── DSP/                 # Audioeffekte
│   │ ├── Compressor_VE.cpp
│   │ ├── DeEsser_VE.cpp
│   │ ├── Equalizer_VE.cpp
│   │ └── Exciter_VE.cpp
│   ├── VocalEnhancerEditor.cpp
│   ├── VocalEnhancerProcessor.cpp
│   └── WaveformDisplay.cpp
```

---

© StudioShine – Alle Rechte vorbehalten.
