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
#include "CustomRotarySliderLook.h"
//==============================================================================
/**
*/
class TapePerformerAudioProcessorEditor  : public juce::AudioProcessorEditor
//,
//public juce::FileDragAndDropTarget,
//private juce::Timer
{
public:
    TapePerformerAudioProcessorEditor (TapePerformerAudioProcessor&);
    ~TapePerformerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void updateToggleState(juce::Button* button, juce::String name);
    
    enum RadioButtonIds
    {
        ModeButtons = 1001,
        KeysAvailableButtons = 1002
    };
    



private:
    
    WaveDisplay waveDisplay;
    EnvelopeDisplay envDisplay;
    
    
    juce::Label modeLabel         { {}, "Modes"};
    CustomToggleButton playModeToggle{"Position", "Pitch"};

    CustomToggleButton playModeToggle2{"Single", "Sustain"};

    juce::Label numKeysLabel        { {}, "Fractions" };
    juce::ToggleButton lessKeysButton  { "12" },
                       moreKeysButton    { "24" };

    CustomRotarySliderLook rotarySliderLook;
    juce::Slider positionSlider;
    juce::Label positionLabel;
    juce::Slider durationSlider;
    juce::Slider spreadSlider;
    juce::Slider gainSlider;
    juce::Slider envShapeSlider;
    
    
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    
    std::unique_ptr<ButtonAttachment> modeAttachment;
    std::unique_ptr<ButtonAttachment> keysAvailableAttachment;
    
//    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
//    std::unique_ptr<ComboBoxAttachment> modeAttachment;
//    std::unique_ptr<ComboBoxAttachment> keysAvailableAttachment;
    
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> positionAttachment;
    std::unique_ptr<SliderAttachment> durationAttachment;
    std::unique_ptr<SliderAttachment> spreadAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    std::unique_ptr<SliderAttachment> envShapeAttachment;
    
    void setSliderParams(juce::Slider& slider);
    void setRotarySliderParams(juce::Slider& slider);
    
    
//    void timerCallback() override
//    {
//        repaint();
//    }
    
    
    TapePerformerAudioProcessor& audioProcessor;
    
//    GrainSound& grainSound;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapePerformerAudioProcessorEditor)
};
