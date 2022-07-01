/*
  ==============================================================================

    FluxModeEditor.cpp
    Created: 1 Jul 2022 8:46:58pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FluxModeEditor.h"

//==============================================================================
FluxModeEditor::FluxModeEditor(TapePerformerAudioProcessor& p) : audioProcessor(p)
{

    setLookAndFeel(&customLookAndFeel);

    firstFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "firstFluxMode", firstFluxModeButton);
    secondFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "secondFluxMode", secondFluxModeButton);
    thirdFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "thirdFluxMode", thirdFluxModeButton);
    fourthFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fourthFluxMode", fourthFluxModeButton);

    setTextButton(firstFluxModeButton, "Forward");
    setTextButton(secondFluxModeButton, "Backward");
    setTextButton(thirdFluxModeButton, "Back and Forth");
    setTextButton(fourthFluxModeButton, "Random");


}

FluxModeEditor::~FluxModeEditor()
{
    setLookAndFeel (nullptr);
}

void FluxModeEditor::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::Slider::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("FluxModeEditor", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void FluxModeEditor::resized()
{
    auto bounds = getLocalBounds();
    auto fullArea = bounds;
    firstFluxModeButton.setBounds(bounds.removeFromTop(juce::jmax (20, fullArea.getHeight() / 6)));
    secondFluxModeButton.setBounds(bounds.removeFromTop(juce::jmax (20, fullArea.getHeight() / 6)));
    thirdFluxModeButton.setBounds(bounds.removeFromTop(juce::jmax (20, fullArea.getHeight() / 6)));
    fourthFluxModeButton.setBounds(bounds.removeFromTop(juce::jmax (20, fullArea.getHeight() / 6)));

    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void FluxModeEditor::setTextButton(juce::Button& button, juce::String text)
{
    button.setButtonText(text);
    button.setRadioGroupId(FluxModeButtons);
    addAndMakeVisible(button);

    button.setClickingTogglesState(true);
}
