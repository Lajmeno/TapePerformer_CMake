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
    setTextButton(thirdFluxModeButton, "Zig-Zag");
    setTextButton(fourthFluxModeButton, "Random");

    addAndMakeVisible(rangeLabel);
    rangeLabel.setText ("Range", juce::dontSendNotification);
    rangeLabel.setFont (juce::Font (14.0f, juce::Font::bold));
    rangeLabel.setJustificationType (juce::Justification::centred);

    rangeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    rangeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(rangeSlider);

    addAndMakeVisible (onOffButton);
    onOffButton.onClick = [this] { updateToggleState (&onOffButton);   };

    onOffButton.setClickingTogglesState (true);


}

FluxModeEditor::~FluxModeEditor()
{
    setLookAndFeel (nullptr);
}

void FluxModeEditor::paint (juce::Graphics& g)
{

    auto color = juce::Colours::black;
    auto bounds = getLocalBounds().toFloat().reduced(6);
    g.setColour(color);
    g.drawRoundedRectangle(bounds, 1.0f, 1.0f);
}

void FluxModeEditor::resized()
{
    auto bounds = getLocalBounds();
    auto fullArea = bounds;
    bounds.removeFromRight(juce::jmax (10, fullArea.getWidth() / 12));
    auto buttonArea = bounds.removeFromRight(juce::jmax (50, fullArea.getWidth() / 2));
    buttonArea.removeFromTop(juce::jmax (20, fullArea.getHeight() / 5));
    buttonArea.removeFromBottom(juce::jmax (20, fullArea.getHeight() / 10));
    firstFluxModeButton.setBounds(buttonArea.removeFromTop(juce::jmax (20, fullArea.getHeight() / 6)));
    secondFluxModeButton.setBounds(buttonArea.removeFromTop(juce::jmax (20, fullArea.getHeight() / 6)));
    thirdFluxModeButton.setBounds(buttonArea.removeFromTop(juce::jmax (20, fullArea.getHeight() / 6)));
    fourthFluxModeButton.setBounds(buttonArea.removeFromTop(juce::jmax (20, fullArea.getHeight() / 6)));

    auto onOffButtonArea = bounds;
    onOffButtonArea.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.8f));
    onOffButton.setBounds(onOffButtonArea.removeFromRight(static_cast<int>(bounds.getWidth() * 0.9f)));

    auto rangeSliderArea = bounds;
    rangeSliderArea.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.1f));
    rangeSlider.setBounds(rangeSliderArea.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.78f)));
    rangeLabel.setBounds(bounds.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.25f)));


}

void FluxModeEditor::setTextButton(juce::Button& button, juce::String text)
{
    button.setButtonText(text);
    button.setRadioGroupId(FluxModeButtons);
    addAndMakeVisible(button);

    button.setClickingTogglesState(true);
}

void FluxModeEditor::updateToggleState (juce::Button* button)
{
    repaint();
}

void FluxModeEditor::paintOverChildren(juce::Graphics& g)
{
    if(!onOffButton.getToggleState())
    {
        auto color = juce::Colours::grey;
        auto bounds = getLocalBounds().toFloat().reduced(6);
        g.setColour(color.withAlpha(0.5f));
        g.fillRoundedRectangle(bounds, 1.0f);
        //g.fillAll(color.withAlpha(0.3f));
    }

}
