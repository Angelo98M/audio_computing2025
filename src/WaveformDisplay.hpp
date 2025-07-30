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


private:
    juce::AudioBuffer<float> audioBuffer;
    int playheadPosition = 0;
};


