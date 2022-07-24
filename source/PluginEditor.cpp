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
    auto parameterArea = getLocalBounds().toFloat();
    auto waveDisplayBorder = parameterArea.removeFromTop(static_cast<int>(fullArea.getHeight() * 0.5f));
    g.setColour(Colours::black);
    g.drawRect(waveDisplayBorder, 3.0f);
    g.drawRect(fullArea, 2.0f);


    auto settingsArea = parameterArea.removeFromLeft(static_cast<int>(fullArea.getWidth() * 0.14)).reduced(6);
    g.drawRoundedRectangle(settingsArea, 8, 1.0f);

    auto transposeArea = parameterArea.removeFromLeft(static_cast<int>(fullArea.getWidth() * 0.11)).reduced(6);
    g.drawRoundedRectangle(transposeArea, 8, 1.0f);

}

void TapePerformerAudioProcessorEditor::resized()
{
    auto fullArea = getLocalBounds();
    auto parameterArea = fullArea;
    auto responseArea = parameterArea.removeFromTop(static_cast<int>(fullArea.getHeight() * 0.5));
    auto gainArea = parameterArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.08f));
    auto fluxModeArea = parameterArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.2));
    auto waveEnvArea = parameterArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.15));
    auto waveEnvParams = waveEnvArea.removeFromBottom(static_cast<int>(parameterArea.getHeight() * 0.3));
    auto mainParamArea = parameterArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.32));
    auto transposeArea = parameterArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.11)).reduced(10);
    auto settingsArea = parameterArea.removeFromRight(static_cast<int>(fullArea.getWidth() * 0.14)).reduced(10);;


    waveDisplay.setBounds(responseArea.reduced(3));
    envDisplay.setBounds(waveEnvArea.reduced(6));
    fluxModeEditor.setBounds(fluxModeArea.reduced(0));


    numKeysLabel.setBounds(settingsArea.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)).reduced(2));
    numKeysMenu.setBounds(settingsArea.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 6)).reduced(2));
    settingsArea.removeFromTop(juce::jmax (3, parameterArea.getHeight() / 16));
    modeLabel.setBounds(settingsArea.removeFromTop(juce::jmax (20, parameterArea.getHeight() / 10)));
    playModeToggle.setBounds(settingsArea.removeFromTop(juce::jmax (16, parameterArea.getHeight() / 10)));
    settingsArea.removeFromTop(juce::jmax (3, parameterArea.getHeight() / 16));
    playModeToggle2.setBounds(settingsArea.removeFromTop(juce::jmax (16, parameterArea.getHeight() / 10)));

    transposeLabel.setBounds(transposeArea.removeFromTop(juce::jmax (40, parameterArea.getHeight() / 4)));
    transposeSlider.setBounds(transposeArea.removeFromTop(juce::jmax (40, parameterArea.getHeight() / 2)));

    mainParamArea.removeFromTop(static_cast<int>(mainParamArea.getHeight() * 0.1f));
    auto positionParamBounds = mainParamArea.removeFromTop(static_cast<int>(mainParamArea.getHeight() * 0.3f));
    positionSlider.setBounds(positionParamBounds);
    positionLabel.setBounds(positionParamBounds.removeFromTop(static_cast<int>(positionParamBounds.getHeight() * 0.3f)));

    auto durationParamBounds = mainParamArea.removeFromTop(static_cast<int>(mainParamArea.getHeight() * 0.428f));
    durationSlider.setBounds(durationParamBounds);
    durationLabel.setBounds(durationParamBounds.removeFromTop(static_cast<int>(durationParamBounds.getHeight() * 0.3f)));

    auto spreadParamBounds = mainParamArea.removeFromTop(static_cast<int>(mainParamArea.getHeight() * 0.75f));;
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

