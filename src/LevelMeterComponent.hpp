#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class LevelMeterComponent : public juce::Component, public juce::Timer
{
public:
    LevelMeterComponent(std::atomic<float>& levelRef) : level(levelRef)
    {
        startTimerHz(30); // ~30 FPS
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        g.fillAll(juce::Colours::black);

        float dB = juce::Decibels::gainToDecibels(currentLevel);
        float normLevel = juce::jmap(dB, -60.0f, 0.0f, 0.0f, 1.0f); // Skalierung

        normLevel = juce::jlimit(0.0f, 1.0f, normLevel);

        auto levelHeight = bounds.getHeight() * (1.0f - normLevel);
        g.setColour(juce::Colours::limegreen);
        g.fillRect(bounds.withY(levelHeight).withHeight(bounds.getHeight() - levelHeight));

        g.setColour(juce::Colours::white);
        g.drawRect(bounds, 2.0f);

    }

    void timerCallback() override
    {
        currentLevel = level.load(); // atomar lesen
        repaint();
    }

private:
    std::atomic<float>& level;
    float currentLevel = 0.0f;
};
