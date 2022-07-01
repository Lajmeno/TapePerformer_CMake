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
#include "CustomLookAndFeel.h"

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

    enum RadioButtonIds
    {
        FluxModeButtons = 1001
    };

    TapePerformerAudioProcessor& audioProcessor;

    CustomLookAndFeel customLookAndFeel;

    juce::TextButton firstFluxModeButton;
    juce::TextButton secondFluxModeButton;
    juce::TextButton thirdFluxModeButton;
    juce::TextButton fourthFluxModeButton;

    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ButtonAttachment> modeAttachment;
    std::unique_ptr<ButtonAttachment> firstFluxButtonAttachment;
    std::unique_ptr<ButtonAttachment> secondFluxButtonAttachment;
    std::unique_ptr<ButtonAttachment> thirdFluxButtonAttachment;
    std::unique_ptr<ButtonAttachment> fourthFluxButtonAttachment;

    void setTextButton(juce::Button& button, juce::String text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FluxModeEditor)
};
