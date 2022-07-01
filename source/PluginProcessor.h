/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Grain.h"
#include "WavetableEnvelope.h"

//==============================================================================
/**
*/
class TapePerformerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TapePerformerAudioProcessor();
    ~TapePerformerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void loadFile();
    void loadFile (const juce::String& path);
    
    int getNumSamplerSounds() { return mSampler.getNumSounds(); }
    

    
    juce::AudioThumbnailCache thumbnailCache;
    juce::AudioThumbnail thumbnail;
    juce::Synthesiser mSampler;

    const int mNumVoices { 6 };
    int midiNoteForNormalPitch = 60;

    
    juce::AudioProcessorValueTreeState apvts;
    
    
    

private:



    juce::AudioFormatManager mFormatManager;
    juce::AudioFormatReader* mFormatReader { nullptr };
    
    std::unique_ptr<juce::FileChooser> chooser;
    
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    float previousGain;
     
    std::atomic<float>* modeParameter = nullptr;
    std::atomic<float>* availableKeysParameter  = nullptr;
    std::atomic<float>* firstFluxParameter  = nullptr;
    std::atomic<float>* secondFluxParameter  = nullptr;
    std::atomic<float>* thirdFluxParameter  = nullptr;
    std::atomic<float>* fourthFluxParameter  = nullptr;
    std::atomic<float>* fluxModeRange  = nullptr;
    std::atomic<float>* positionParameter = nullptr;
    std::atomic<float>* durationParameter  = nullptr;
    std::atomic<float>* spreadParameter = nullptr;
    std::atomic<float>* gainParameter  = nullptr;
    std::atomic<float>* envelopeShapeParameter  = nullptr;
    std::atomic<float>* transposeParameter  = nullptr;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapePerformerAudioProcessor)
};
