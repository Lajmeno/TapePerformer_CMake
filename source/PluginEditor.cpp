/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <utility>
#include "WaveDisplay.h"

//==============================================================================
TapePerformerAudioProcessorEditor::TapePerformerAudioProcessorEditor (TapePerformerAudioProcessor& p)
    : AudioProcessorEditor (&p), waveDisplay(p), audioProcessor (p)
{

    addAndMakeVisible(waveDisplay);
    addAndMakeVisible(envDisplay);
    setResizable(true, true);
    setResizeLimits(800, 250, 1200, 400);
    setSize (800, 300);
    
    
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "playMode", positionButton);
    
    keysAvailableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "numKeys", lessKeysButton);
    
    
    positionAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "position", positionSlider);
    durationAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "duration", durationSlider);
    spreadAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "spread", spreadSlider);
    gainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "gain", gainSlider);
    envShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "envShape", envShapeSlider);
    
    setSliderParams(positionSlider);
    setSliderParams(durationSlider);
    setSliderParams(spreadSlider);
    
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(gainSlider);

    setSliderParams(envShapeSlider);
    
    
    addAndMakeVisible (modeLabel);

    addAndMakeVisible (positionButton);
    addAndMakeVisible (pitchButton);
    positionButton.onClick = [this] { updateToggleState (&positionButton,   "Position");   };
    pitchButton.onClick = [this] { updateToggleState (&pitchButton, "Pitch"); };
    
//    positionButton.setState(juce::Button::ButtonState(1));

    positionButton.setRadioGroupId (ModeButtons);
    pitchButton.setRadioGroupId (ModeButtons);
    
    addAndMakeVisible (numKeysLabel);

    addAndMakeVisible (lessKeysButton);
    addAndMakeVisible (moreKeysButton);
    lessKeysButton.onClick = [this] { updateToggleState (&lessKeysButton,   "12 Keys");   };
    moreKeysButton.onClick = [this] { updateToggleState (&moreKeysButton, "24 Keys"); };

    lessKeysButton  .setRadioGroupId (KeysAvailableButtons);
    moreKeysButton.setRadioGroupId (KeysAvailableButtons);
  
}

TapePerformerAudioProcessorEditor::~TapePerformerAudioProcessorEditor()
{
}

//==============================================================================
void TapePerformerAudioProcessorEditor::paint (juce::Graphics& g)
{

    g.fillAll(juce::Colours::darkgrey);


}

void TapePerformerAudioProcessorEditor::resized()
{

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.5));
    auto gainArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.1));
    auto waveEnvArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.2));
    auto waveEnvParams = waveEnvArea.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.3));
    
    waveDisplay.setBounds(responseArea.reduced(4));
    envDisplay.setBounds(waveEnvArea.reduced(4));

    auto parameterArea = bounds;
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto generalSettings = parameterArea.removeFromLeft(juce::jmax (40, bounds.getWidth() / 6));
    auto positionArea = parameterArea.removeFromLeft(juce::jmax (60, bounds.getWidth() / 4));
    auto durationArea = parameterArea.removeFromLeft(juce::jmax (60, bounds.getWidth() / 4));
    auto spreadArea = parameterArea.removeFromLeft(juce::jmax (60, bounds.getWidth() / 4));
    
    modeLabel.setBounds(generalSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    positionButton.setBounds(generalSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    pitchButton.setBounds(generalSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    
    numKeysLabel.setBounds(generalSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    lessKeysButton.setBounds(generalSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    moreKeysButton.setBounds(generalSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    
    positionSlider.setBounds(positionArea.removeFromTop(static_cast<int>(positionArea.getHeight() * 0.5)));
    durationSlider.setBounds(durationArea.removeFromTop(static_cast<int>(durationArea.getHeight() * 0.5)));
    spreadSlider.setBounds(spreadArea.removeFromTop(static_cast<int>(spreadArea.getHeight() * 0.5)));
    gainSlider.setBounds(gainArea);
    envShapeSlider.setBounds(waveEnvParams);

}


void TapePerformerAudioProcessorEditor::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(slider);
    
}

void TapePerformerAudioProcessorEditor::updateToggleState (juce::Button* button, juce::String name)
{
    auto state = button->getToggleState();
    juce::String stateString = state ? "ON" : "OFF";

    //juce::Logger::outputDebugString (std::move(name) + " Button changed to " + stateString);
}
