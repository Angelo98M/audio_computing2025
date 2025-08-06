//
// Created by Angel on 30.07.2025.
//

#include "WaveformDisplay.hpp"

WaveformDisplay::WaveformDisplay(bool isStandalone) : isStandalone(isStandalone) {

}


void WaveformDisplay::paint(juce::Graphics &g)
{


    const int width = getWidth();
    const int height = getHeight();
    if (isStandalone) {
        g.fillAll(juce::Colours::black);
        if (audioBuffer.getNumSamples() == 0)
            return;
        const int numSamples = audioBuffer.getNumSamples();

        auto* data = audioBuffer.getReadPointer(0); // Kanal 0

        const int samplesPerPixel = juce::jmax(1, numSamples / width);

        // === Wellenform zeichnen ===
        g.setColour(juce::Colours::white);

        for (int x = 0; x < width; ++x)
        {
            const int startSample = x * samplesPerPixel;
            float minSample = 1.0f;
            float maxSample = -1.0f;

            for (int i = 0; i < samplesPerPixel; ++i)
            {
                const int sampleIndex = startSample + i;
                if (sampleIndex < numSamples)
                {
                    const float sample = data[sampleIndex];
                    if (sample < minSample) minSample = sample;
                    if (sample > maxSample) maxSample = sample;
                }
            }

            const float y1 = juce::jmap(minSample, -1.0f, 1.0f, (float)height, 0.0f);
            const float y2 = juce::jmap(maxSample, -1.0f, 1.0f, (float)height, 0.0f);
            g.drawLine((float)x, y1, (float)x, y2);
        }

        // === Playhead zeichnen ===
        g.setColour(juce::Colours::red);
        const float playheadX = (float)width * playheadPosition;
        g.drawLine(playheadX, 0.0f, playheadX, (float)height, 2.0f);
    }

    // === ADSR zeichnen ===
    g.setColour(juce::Colours::yellow.withAlpha(0.8f));

    juce::Path adsrPath;
    auto bounds = getLocalBounds().toFloat().reduced(5);
    auto widthF = bounds.getWidth();
    auto heightF = bounds.getHeight();

    float totalMs = attackMs + decayMs + releaseMs + 1.0f;
    float attackX = (attackMs / totalMs) * widthF;
    float decayX = (decayMs / totalMs) * widthF;
    float releaseX = (releaseMs / totalMs) * widthF;
    float sustainY = bounds.getBottom() - (sustainLevel * heightF);

    adsrPath.startNewSubPath(bounds.getX(), bounds.getBottom());                   // Start
    adsrPath.lineTo(bounds.getX() + attackX, bounds.getY());                       // Attack
    adsrPath.lineTo(bounds.getX() + attackX + decayX, sustainY);                   // Decay
    adsrPath.lineTo(bounds.getRight() - releaseX, sustainY);                       // Sustain
    adsrPath.lineTo(bounds.getRight(), bounds.getBottom());                        // Release

    g.strokePath(adsrPath, juce::PathStrokeType(2.0f));
}


bool WaveformDisplay::hasBuffer() {
    return  audioBuffer.getNumSamples() > 0;
}
