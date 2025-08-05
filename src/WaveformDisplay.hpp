#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay() = default;

    void setAudioBuffer(const juce::AudioBuffer<float>& bufferToUse)
    {
        audioBuffer.makeCopyOf(bufferToUse);
        repaint();
    }

    void paint(juce::Graphics& g) override;

    void resized() override {}

    void setPlayheadPosition(int samplePosition)
    {
        playheadPosition = samplePosition;
        repaint();
    }
    void setADSR(float a, float d, float s, float r)
    {
        attackMs = a;
        decayMs = d;
        sustainLevel = s;
        releaseMs = r;
        repaint();
    }

    bool hasBuffer();


private:
    juce::AudioBuffer<float> audioBuffer;
    int playheadPosition = 0;
    float attackMs = 100.0f;
    float decayMs = 100.0f;
    float sustainLevel = 0.8f;
    float releaseMs = 200.0f;



};


