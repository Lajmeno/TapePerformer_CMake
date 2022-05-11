/*
  ==============================================================================

    Grain.h
    Created: 4 Jan 2022 12:25:01pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WavetableEnvelope.h"



class GrainSound : public juce::SynthesiserSound
{
public:
    GrainSound (const juce::String& name,
                juce::AudioFormatReader& source,
                  const juce::BigInteger& midiNotes,
                  int midiNoteForNormalPitch,
                  double attackTimeSecs,
                  double releaseTimeSecs,
                  double maxSampleLengthSeconds);

    /** Destructor. */
    ~GrainSound() override;
    
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    int getNumOfKeysAvailable() { return numOfKeysAvailable; }
    int getDurationParam() { return durationParam; }
    double getPositionsParam() { return positionParam; }
    float getSpreadParam() { return spreadParam; }
    
    void updateParams(const bool mode, const bool availableKeys, const float position, const float duration, const float spread);
  
    
    
    
    
private:
    friend class GrainVoice;
    
    juce::String name;
    std::unique_ptr<juce::AudioBuffer<float>> data;
    double sourceSampleRate;
    juce::BigInteger midiNotes;
    int length = 0, midiRootNote = 0;
    
    juce::ADSR::Parameters params;
    
    bool pitchModeParam = false;
    double positionParam = 11025;
    float transpositionParam = 60.0;   //midiRoot
    double durationParam = 36075;
    int numOfKeysAvailable = 12;
    float spreadParam = 0.2;
    
//    JUCE_LEAK_DETECTOR (GrainSound)
};


class GrainVoice : public juce::SynthesiserVoice
{
public:
    /** Creates a SamplerVoice. */
    GrainVoice();

    /** Destructor. */
    ~GrainVoice() override;

    //==============================================================================
    bool canPlaySound (juce::SynthesiserSound*) override;

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    
//    bool isKeyDown() const noexcept                             { return keyIsDown; }

    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;

    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;
    using juce::SynthesiserVoice::renderNextBlock;
    
    double setStartPosition(GrainSound* sound, int midiNoteNumber);
    
    
    double getPosition();
    
    void createWavetableEnv();

    
    
private:
    double sampleRate = 0;
    bool keyIsDown = false;
    
    double startPosition = 0;
    int currentMiniNumber = 0;
    
    double pitchRatio = 0;
    double sourceSamplePosition = 0;
    double numPlayedSamples = 0;
    float lgain = 0, rgain = 0;
    

    
    

    juce::ADSR adsr;
    
    WavetableEnvelope envCurve;
//    juce::AudioSampleBuffer envTable;
    
    
    
//    JUCE_LEAK_DETECTOR (GrainVoice)
};


