# 🎙️ Simply better Voice — Vocal Enhancer

**Version:** 0.0.1  
**Autor:** StudioShine

## 📌 Projektbeschreibung

**Simply better Voice** ist ein Audio-Plugin (AU, VST3, Standalone), das speziell für die Optimierung von Sprachaufnahmen entwickelt wurde. Ziel ist es, mit wenigen Klicks eine angenehmere und klarere Sprachqualität zu erreichen – ideal für Podcasts, Videovertonung oder Voiceovers.

Es bietet grundlegende Audio-Effekte wie:
- **Kompression**
- **Equalizer**
- **De-Esser**
- **Exciter**
- **Waveform-Anzeige**

Das Plugin basiert auf [JUCE](https://juce.com) und ist vollständig in C++20 entwickelt.

---

## ⚙️ Installation

### Voraussetzungen

- **Betriebssystem:** Windows
- **CMake:** ≥ 3.31
- **Compiler:** C++20-kompatibel (z. B. Clang, MSVC, GCC)
- **Abhängigkeiten:**  
  - [JUCE Framework](https://github.com/juce-framework/JUCE) (im Repository unter `extern/JUCE` eingebunden)

### Repository klonen

```bash
git clone --recurse-submodules https://github.com/dein-benutzername/vocal-enhancer.git
cd vocal-enhancer
```

> **Hinweis:** Das `--recurse-submodules` ist notwendig, da JUCE als Submodul eingebunden ist.

---

## 🏗️ Build-Anleitung

### Mit CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Das erstellt:
- das Plugin in den Formaten **VST3**
- eine **Standalone-Version** der App

Je nach System werden die Plugins nach erfolgreichem Build automatisch an den üblichen Plugin-Speicherorten installiert.

---

## 🧪 Plugin-Formate

Dieses Projekt erzeugt folgende Formate:
- ✅ **VST3**
- ✅ **Standalone-App**

> **Nicht enthalten:** AAX, AUv3, VST2 (VST2 ist nicht mehr offiziell unterstützt)

---

## 📁 Projektstruktur

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
Für die Dokumentation [hier](Documentation/Dokumentation.md)

---

## 🔧 Konfiguration

Das Plugin benötigt **keine zusätzlichen Umgebungsvariablen** oder Konfigurationsdateien. Es ist sofort nach dem Build einsatzbereit.

