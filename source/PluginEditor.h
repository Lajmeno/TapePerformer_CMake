/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveDisplay.h"
#include "EnvelopeDisplay.h"
#include "CustomToggleButton.h"
#include "CustomLookAndFeel.h"
#include "FluxModeEditor.h"
//==============================================================================
/**
*/
class TapePerformerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    TapePerformerAudioProcessorEditor (TapePerformerAudioProcessor&);
    ~TapePerformerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    enum RadioButtonIds
    {
        FluxModeButtons = 1001
    };
    

private:
    
    WaveDisplay waveDisplay;
    EnvelopeDisplay envDisplay;
    FluxModeEditor fluxModeEditor;
    
    
    juce::Label modeLabel         { {}, "Modes"};
    CustomToggleButton playModeToggle{"Position", "Pitch"};

    CustomToggleButton playModeToggle2{"Single", "Sustain"};


    juce::Font textFont   { 14.0f, juce::Font::bold};


    juce::Label numKeysLabel        { {}, "Fractions" };
    juce::ComboBox numKeysMenu;

    CustomLookAndFeel customLookAndFeel;
    juce::Slider positionSlider;
    juce::Slider durationSlider;
    juce::Slider spreadSlider;
    juce::Slider gainSlider;
    juce::Slider envShapeSlider;
    juce::Slider transposeSlider;

    juce::Label positionLabel;
    juce::Label durationLabel;
    juce::Label spreadLabel;
    juce::Label transposeLabel;
    juce::Label gainLabel;
    juce::Label envShapeLabel;


    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<ButtonAttachment> modeAttachment;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> keysAvailableAttachment;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> positionAttachment;
    std::unique_ptr<SliderAttachment> durationAttachment;
    std::unique_ptr<SliderAttachment> spreadAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> envShapeAttachment;
    std::unique_ptr<SliderAttachment> transposeAttachment;
    
    void setSliderParams(juce::Slider& slider, juce::Label& label, juce::String name);
    void setRotarySliderParams(juce::Slider& slider);
    void setTextButton(juce::Button& button, juce::String text);

    
    TapePerformerAudioProcessor& audioProcessor;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapePerformerAudioProcessorEditor)
};
