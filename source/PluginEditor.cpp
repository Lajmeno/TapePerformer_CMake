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

    setLookAndFeel(&customLookAndFeel);

    addAndMakeVisible(waveDisplay);
    addAndMakeVisible(envDisplay);
    setResizable(true, true);
    setResizeLimits(800, 250, 1200, 400);
    setSize (800, 300);
    
    
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "playMode", playModeToggle);
    
    keysAvailableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "numKeys", lessKeysButton);

    firstFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "firstFluxMode", firstFluxModeButton);
    secondFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "secondFluxMode", secondFluxModeButton);
    thirdFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "thirdFluxMode", thirdFluxModeButton);
    fourthFluxButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "fourthFluxMode", fourthFluxModeButton);

    positionAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "position", positionSlider);
    durationAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "duration", durationSlider);
    spreadAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "spread", spreadSlider);
    gainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "gain", gainSlider);
    envShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "envShape", envShapeSlider);
    
    setSliderParams(positionSlider);
    setRotarySliderParams(durationSlider);
    setRotarySliderParams(spreadSlider);

    addAndMakeVisible (positionLabel);
    positionLabel.setText ("Position", juce::dontSendNotification);
    positionLabel.attachToComponent (&positionSlider, false);
    positionLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    positionLabel.setJustificationType (juce::Justification::centred);
    
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(gainSlider);

    setTextButton(firstFluxModeButton, "Forward");
    setTextButton(secondFluxModeButton, "Backward");
    setTextButton(thirdFluxModeButton, "Back and Forth");
    setTextButton(fourthFluxModeButton, "Random");


    setSliderParams(envShapeSlider);
    
    
    addAndMakeVisible (modeLabel);
    modeLabel.setFont (juce::Font (12.0f, juce::Font::bold));
    modeLabel.setJustificationType (juce::Justification::centred);

    addAndMakeVisible(playModeToggle);
    addAndMakeVisible(playModeToggle2);
    
    addAndMakeVisible (numKeysLabel);

    addAndMakeVisible (lessKeysButton);
    addAndMakeVisible (moreKeysButton);
    lessKeysButton.onClick = [this] { updateToggleState (&lessKeysButton,   "12 Keys");   };
    moreKeysButton.onClick = [this] { updateToggleState (&moreKeysButton, "24 Keys"); };

    lessKeysButton.setRadioGroupId (KeysAvailableButtons);
    moreKeysButton.setRadioGroupId (KeysAvailableButtons);
  
}

TapePerformerAudioProcessorEditor::~TapePerformerAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

//==============================================================================
void TapePerformerAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    auto bgHue = juce::Colour::fromString("#FFB830").getHue();
    auto backgroundColour = juce::Colour(bgHue, 0.5f, .0f, 1.0f);

    g.fillAll(juce::Colour::fromString("#187498").brighter(.3f));

    auto fullArea = getLocalBounds().toFloat();
    auto waveDisplayBorder = fullArea.removeFromTop(static_cast<int>(fullArea.getHeight() * 0.5f));
    g.setColour(Colours::black);
    g.drawRect(waveDisplayBorder, 3.0f);
    g.drawRect(fullArea, 2.0f);

    auto gainAreaBorder = fullArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.09f));
    g.drawLine(gainAreaBorder.getX(), gainAreaBorder.getY() + gainAreaBorder.getBottom() * 0.05f, gainAreaBorder.getX(), gainAreaBorder.getBottom() * 0.95f, 1.25f);

    auto waveAreaBorder = fullArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.215f));
    g.drawLine(waveAreaBorder.getX(), waveAreaBorder.getY() + waveAreaBorder.getBottom() * 0.05f, waveAreaBorder.getX(), waveAreaBorder.getBottom() * 0.95f, 1.25f);




}

void TapePerformerAudioProcessorEditor::resized()
{

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.5));
    auto gainArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.1));
    auto fluxModeArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.2));
    auto waveEnvArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.2));
    auto waveEnvParams = waveEnvArea.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.3));
    
    waveDisplay.setBounds(responseArea.reduced(3));
    envDisplay.setBounds(waveEnvArea.reduced(4));

    auto parameterArea = bounds;
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto extraSettings = parameterArea.removeFromLeft(juce::jmax (80, bounds.getWidth() / 6)).reduced(4);
    
    auto generalSettings = parameterArea.removeFromLeft(juce::jmax (40, bounds.getWidth() / 6));
    auto paramArea = parameterArea.removeFromLeft(juce::jmax (150, bounds.getWidth() / 4));
    //auto durationArea = parameterArea.removeFromLeft(juce::jmax (60, bounds.getWidth() / 4));
    //auto spreadArea = parameterArea.removeFromLeft(juce::jmax (60, bounds.getWidth() / 4));
    
    modeLabel.setBounds(extraSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 10)));
    
    playModeToggle.setBounds(extraSettings.removeFromTop(juce::jmax (16, parameterArea.getHeight() / 10)));
    extraSettings.removeFromTop(juce::jmax (3, parameterArea.getHeight() / 16));
    playModeToggle2.setBounds(extraSettings.removeFromTop(juce::jmax (16, parameterArea.getHeight() / 10)));

    numKeysLabel.setBounds(generalSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    lessKeysButton.setBounds(generalSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    moreKeysButton.setBounds(generalSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));

    positionLabel.setBounds(paramArea.removeFromTop(static_cast<int>(paramArea.getHeight() * 0.2f)));
    positionSlider.setBounds(paramArea.removeFromTop(static_cast<int>(paramArea.getHeight() * 0.4f)));
    paramArea.removeFromTop(static_cast<int>(paramArea.getHeight() * 0.1f));
    durationSlider.setBounds(paramArea.removeFromLeft(static_cast<int>(paramArea.getWidth() * 0.3f)));
    spreadSlider.setBounds(paramArea);
    paramArea.removeFromBottom(static_cast<int>(paramArea.getHeight() * 0.2f));

    firstFluxModeButton.setBounds(fluxModeArea.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    secondFluxModeButton.setBounds(fluxModeArea.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    thirdFluxModeButton.setBounds(fluxModeArea.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    fourthFluxModeButton.setBounds(fluxModeArea.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));

    gainSlider.setBounds(gainArea);

    envShapeSlider.setBounds(waveEnvParams);

}


void TapePerformerAudioProcessorEditor::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(slider);
}




void TapePerformerAudioProcessorEditor::setRotarySliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 0);
    addAndMakeVisible(slider);

}

void TapePerformerAudioProcessorEditor::setTextButton(juce::Button& button, juce::String string)
{
    button.setButtonText(string);
    button.setRadioGroupId(FluxModeButtons);
    addAndMakeVisible(button);

    button.setClickingTogglesState(true);
}

void TapePerformerAudioProcessorEditor::updateToggleState (juce::Button* button, juce::String name)
{
    auto state = button->getToggleState();
    juce::String stateString = state ? "ON" : "OFF";

    //juce::Logger::outputDebugString (std::move(name) + " Button changed to " + stateString);
}
