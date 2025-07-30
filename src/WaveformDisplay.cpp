//
// Created by Angel on 30.07.2025.
//

#include "WaveformDisplay.hpp"

void WaveformDisplay::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::black);

    if (audioBuffer.getNumSamples() == 0)
        return;

    g.setColour(juce::Colours::white);

    const int width = getWidth();
    const int height = getHeight();
    const int numSamples = audioBuffer.getNumSamples();
    const int numChannels = audioBuffer.getNumChannels();

    auto* data = audioBuffer.getReadPointer(0); // nur Kanal 0 für Anzeige

    const int samplesPerPixel = juce::jmax(1, numSamples / width);

    for (int x = 0; x < width; ++x)
    {
        const int startSample = x * samplesPerPixel;
        float minSample = 1.0f;
        float maxSample = -1.0f;

        for (int i = 0; i < samplesPerPixel; ++i)
        {
            const float sample = data[startSample + i];
            if (sample < minSample) minSample = sample;
            if (sample > maxSample) maxSample = sample;
        }

        const float y1 = juce::jmap(minSample, -1.0f, 1.0f, (float)height, 0.0f);
        const float y2 = juce::jmap(maxSample, -1.0f, 1.0f, (float)height, 0.0f);
        g.drawLine((float)x, y1, (float)x, y2);

        // === Playhead Position ===
        if (audioBuffer.getNumSamples() > 0)
        {
            const int width = getWidth();
            const int totalSamples = audioBuffer.getNumSamples();

            const float playheadX = (totalSamples > 0) ? (width * playheadPosition) / (float)totalSamples : 0.0f;

            g.setColour(juce::Colours::red);
            g.drawLine(playheadX, 0.0f, playheadX, (float)getHeight(), 2.0f);
        }

    }
}
