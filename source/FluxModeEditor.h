/*
  ==============================================================================

    FluxModeEditor.h
    Created: 1 Jul 2022 8:46:58pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class FluxModeEditor  : public juce::Component
{
public:
    FluxModeEditor(TapePerformerAudioProcessor&);
    ~FluxModeEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:

    TapePerformerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FluxModeEditor)
};
