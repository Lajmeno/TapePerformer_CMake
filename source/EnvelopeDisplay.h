/*
  ==============================================================================

    EnvelopeDisplay.h
    Created: 29 Jan 2022 2:15:43pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WavetableEnvelope.h"

//==============================================================================
/*
*/
class EnvelopeDisplay  : public juce::Component,
                         private juce::Timer
{
public:
    EnvelopeDisplay();
    ~EnvelopeDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    

private:
    void timerCallback() override
    {
        if(envShapeValue != WavetableEnvelope::envelopeShape)
        {
            repaint();
            envShapeValue = WavetableEnvelope::envelopeShape;
        }
    }
    void drawWaveform(juce::Graphics& g, const juce::Rectangle<int>& waveDisplayArea);

    float envShapeValue = 1.0f;
    
    WavetableEnvelope envCurve;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeDisplay)
};
