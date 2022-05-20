/*
  ==============================================================================

    EnvelopeDisplay.cpp
    Created: 29 Jan 2022 2:15:43pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EnvelopeDisplay.h"

//==============================================================================
EnvelopeDisplay::EnvelopeDisplay() : envCurve()
{

    envCurve.setFrequency(2, 44100);
    startTimer(40);

}

EnvelopeDisplay::~EnvelopeDisplay()
{
}

void EnvelopeDisplay::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    
    auto bounds = getLocalBounds();
    
    //auto waveDisplayArea = bounds.removeFromTop(bounds.getHeight()* 0.75);
    
    

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);

    auto waveDrawArea = bounds.reduced(4);
    drawWaveform(g, bounds);

    //g.fillRect (waveDrawArea);
    
}

void EnvelopeDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


void EnvelopeDisplay::drawWaveform(juce::Graphics& g, const juce::Rectangle<int>& waveDrawArea)
{
    envCurve.createWavetableEnv();
    
    auto wavetable = envCurve.getWavetable();
    auto* samples = wavetable.getReadPointer(0);
    
    for (int i = 0; i < wavetable.getNumSamples(); i++){
        g.drawVerticalLine((int)(((waveDrawArea.getWidth() - 4) / 2048.0f) * (i + 15)),
                           (waveDrawArea.getHeight() + 2) - samples[i] * waveDrawArea.getHeight(),
                           waveDrawArea.getHeight() - 2);
    }

}
