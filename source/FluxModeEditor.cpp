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

    rangeAttachment =std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "fluxModeRange", rangeSlider);

    firstFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "firstFluxMode", firstFluxModeButton);
    secondFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "secondFluxMode", secondFluxModeButton);
    thirdFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "thirdFluxMode", thirdFluxModeButton);
    fourthFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fourthFluxMode", fourthFluxModeButton);

    setTextButton(firstFluxModeButton, "Forward");
    setTextButton(secondFluxModeButton, "Backward");
    setTextButton(thirdFluxModeButton, "Back and Forth");
    setTextButton(fourthFluxModeButton, "Random");

    addAndMakeVisible(rangeLabel);
    rangeLabel.setText ("Range", juce::dontSendNotification);
    //positionLabel.attachToComponent (&slider, false);
    rangeLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    rangeLabel.setJustificationType (juce::Justification::centred);
    rangeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    rangeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(rangeSlider);


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


    g.fillAll (getLookAndFeel().findColour (customLookAndFeel.backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("FluxModeEditor", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
                */
}

void FluxModeEditor::resized()
{
    auto bounds = getLocalBounds();
    auto fullArea = bounds;
    auto buttonArea = bounds.removeFromRight(juce::jmax (50, fullArea.getWidth() / 2));
    firstFluxModeButton.setBounds(buttonArea.removeFromTop(juce::jmax (40, fullArea.getHeight() / 4)));
    secondFluxModeButton.setBounds(buttonArea.removeFromTop(juce::jmax (40, fullArea.getHeight() / 4)));
    thirdFluxModeButton.setBounds(buttonArea.removeFromTop(juce::jmax (40, fullArea.getHeight() / 4)));
    fourthFluxModeButton.setBounds(buttonArea.removeFromTop(juce::jmax (40, fullArea.getHeight() / 4)));


    rangeSlider.setBounds(bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.8f)));
    rangeLabel.setBounds(bounds);

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
