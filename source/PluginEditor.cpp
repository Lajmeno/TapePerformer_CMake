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
    : AudioProcessorEditor (&p), waveDisplay(p), audioProcessor (p), fluxModeEditor(p)
{

    setLookAndFeel(&customLookAndFeel);

    addAndMakeVisible(waveDisplay);
    addAndMakeVisible(envDisplay);
    addAndMakeVisible(fluxModeEditor);
    setResizable(true, true);
    setResizeLimits(800, 250, 1200, 400);
    setSize (800, 300);
    
    
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "playMode", playModeToggle);
    keysAvailableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "numKeys", numKeysMenu);

    positionAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "position", positionSlider);
    durationAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "duration", durationSlider);
    spreadAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "spread", spreadSlider);
    gainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "gain", gainSlider);
    envShapeAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "envShape", envShapeSlider);
    transposeAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "transpose", transposeSlider);


    setSliderParams(positionSlider, positionLabel, "Position");
    setSliderParams(durationSlider, durationLabel, "Duration");
    setSliderParams(spreadSlider, spreadLabel, "Spread");
    
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(gainSlider);

    addAndMakeVisible (gainLabel);
    gainLabel.setFont (textFont);
    gainLabel.setJustificationType (juce::Justification::centred);
    gainLabel.setText("Gain", juce::dontSendNotification);

    //setSliderParams(envShapeSlider);
    envShapeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    envShapeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(envShapeSlider);
    
    
    addAndMakeVisible (modeLabel);
    modeLabel.setFont (textFont);
    modeLabel.setJustificationType (juce::Justification::centred);

    addAndMakeVisible(playModeToggle);
    addAndMakeVisible(playModeToggle2);


    numKeysLabel.setFont(textFont);
    numKeysLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (numKeysLabel);

    numKeysMenu.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (numKeysMenu);
    numKeysMenu.addItem ("12",  1);
    numKeysMenu.addItem ("24",   2);
    numKeysMenu.addItem ("48", 3);
    numKeysMenu.addItem ("96", 4);

    numKeysMenu.setSelectedId (1);

    transposeSlider.setSliderStyle(juce::Slider::Rotary);
    addAndMakeVisible (transposeSlider);
    transposeSlider.setRange (-48, 48, 1);          // [1]
    transposeSlider.setTextValueSuffix (" St");     // [2]
    transposeSlider.onValueChange = [this] { transposeSlider.setValue (transposeSlider.getValue(), juce::dontSendNotification); };

    transposeSlider.setValue (0);
    transposeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 160, transposeSlider.getTextBoxHeight());

    addAndMakeVisible (transposeLabel);
    transposeLabel.setFont (textFont);
    transposeLabel.setJustificationType (juce::Justification::centred);
    transposeLabel.setText("Transpose", juce::dontSendNotification);

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

    g.fillAll(getLookAndFeel().findColour (customLookAndFeel.backgroundColourId));

    auto fullArea = getLocalBounds().toFloat();
    auto waveDisplayBorder = fullArea.removeFromTop(static_cast<int>(fullArea.getHeight() * 0.5f));
    g.setColour(Colours::black);
    g.drawRect(waveDisplayBorder, 3.0f);
    g.drawRect(fullArea, 2.0f);

    /*
    auto gainAreaBorder = fullArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.09f));
    g.drawLine(gainAreaBorder.getX(), gainAreaBorder.getY() + gainAreaBorder.getBottom() * 0.05f, gainAreaBorder.getX(), gainAreaBorder.getBottom() * 0.95f, 1.25f);
     */

    //auto fluxAreaBorder = fullArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.215f)).reduced(6);;
    //g.drawLine(waveAreaBorder.getX(), waveAreaBorder.getY() + waveAreaBorder.getBottom() * 0.05f, waveAreaBorder.getX(), waveAreaBorder.getBottom() * 0.95f, 1.25f);
    //g.drawRoundedRectangle(fluxAreaBorder, 8, 1.5f);

    auto modeParamsArea = fullArea.removeFromLeft(juce::jmax(80.0f, fullArea.getWidth() / 6)).reduced(6);
    g.drawRoundedRectangle(modeParamsArea, 8, 1.5f);

}

void TapePerformerAudioProcessorEditor::resized()
{

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.5));
    auto gainArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.08f));
    auto fluxModeArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.2));
    auto waveEnvArea = bounds.removeFromRight(static_cast<int>(bounds.getWidth() * 0.2));
    auto waveEnvParams = waveEnvArea.removeFromBottom(static_cast<int>(bounds.getHeight() * 0.3));
    
    waveDisplay.setBounds(responseArea.reduced(3));
    envDisplay.setBounds(waveEnvArea.reduced(4));
    fluxModeEditor.setBounds(fluxModeArea.reduced(0));

    auto parameterArea = bounds;
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto extraSettings = parameterArea.removeFromLeft(juce::jmax (80, bounds.getWidth() / 6)).reduced(8);
    
    auto generalSettings = parameterArea.removeFromLeft(juce::jmax (40, bounds.getWidth() / 6));
    auto paramArea = parameterArea.removeFromLeft(juce::jmax (250, bounds.getWidth() / 4));
    //auto durationArea = parameterArea.removeFromLeft(juce::jmax (60, bounds.getWidth() / 4));
    //auto spreadArea = parameterArea.removeFromLeft(juce::jmax (60, bounds.getWidth() / 4));
    
    modeLabel.setBounds(extraSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 10)));
    
    playModeToggle.setBounds(extraSettings.removeFromTop(juce::jmax (16, parameterArea.getHeight() / 10)));
    extraSettings.removeFromTop(juce::jmax (3, parameterArea.getHeight() / 16));
    playModeToggle2.setBounds(extraSettings.removeFromTop(juce::jmax (16, parameterArea.getHeight() / 10)));

    numKeysLabel.setBounds(extraSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));
    numKeysMenu.setBounds(extraSettings.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)));

    transposeLabel.setBounds(generalSettings.removeFromTop(juce::jmax (40, parameterArea.getHeight() / 4)));
    transposeSlider.setBounds(generalSettings.removeFromTop(juce::jmax (40, parameterArea.getHeight() / 2)));

    //positionLabel.setBounds(paramArea.removeFromTop(static_cast<int>(paramArea.getHeight() * 0.2f)));
    paramArea.removeFromTop(static_cast<int>(paramArea.getHeight() * 0.1f));
    auto positionParamBounds = paramArea.removeFromTop(static_cast<int>(paramArea.getHeight() * 0.3f));
    positionSlider.setBounds(positionParamBounds);
    positionLabel.setBounds(positionParamBounds.removeFromTop(static_cast<int>(positionParamBounds.getHeight() * 0.3f)));

    auto durationParamBounds = paramArea.removeFromTop(static_cast<int>(paramArea.getHeight() * 0.428f));
    durationSlider.setBounds(durationParamBounds);
    durationLabel.setBounds(durationParamBounds.removeFromTop(static_cast<int>(durationParamBounds.getHeight() * 0.3f)));

    auto spreadParamBounds = paramArea.removeFromTop(static_cast<int>(paramArea.getHeight() * 0.75f));;
    spreadSlider.setBounds(spreadParamBounds);
    spreadLabel.setBounds(spreadParamBounds.removeFromTop(static_cast<int>(spreadParamBounds.getHeight() * 0.35f)));

    gainLabel.setBounds(gainArea.removeFromTop(juce::jmax (20, gainArea.getHeight() / 10)));
    gainSlider.setBounds(gainArea);

    envShapeSlider.setBounds(waveEnvParams);

}


void TapePerformerAudioProcessorEditor::setSliderParams(juce::Slider& slider, juce::Label& label, juce::String name)
{
    addAndMakeVisible (label);
    label.setText (name, juce::dontSendNotification);
    //positionLabel.attachToComponent (&slider, false);
    label.setFont (textFont);
    label.setJustificationType (juce::Justification::centred);

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

void TapePerformerAudioProcessorEditor::setTextButton(juce::Button& button, juce::String text)
{
    button.setButtonText(text);
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
