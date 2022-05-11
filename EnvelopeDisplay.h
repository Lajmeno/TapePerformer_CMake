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
        //repaint();
    }
    void drawWaveform(juce::Graphics& g, const juce::Rectangle<int>& waveDisplayArea);
    
    WavetableEnvelope envCurve;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnvelopeDisplay)
};
