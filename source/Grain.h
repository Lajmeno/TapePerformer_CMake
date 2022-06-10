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
    double getDurationParam() { return durationParam; }
    double getPositionsParam() { return positionParam; }
    float getSpreadParam() { return spreadParam; }
    
    void updateParams(float mode, float availableKeys, double position, double duration, float spread);
  
    
    
    
    
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
    float transpositionParam = 60.0f;   //midiRoot
    double durationParam = 36075;
    int numOfKeysAvailable = 12;
    float spreadParam = 0.2f;
    
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
    
    double setStartPosition(GrainSound* sound, bool newlyStarted);
    void setEnvelopeFrequency(GrainSound* sound);
    
    
    double getPosition();
    
    void createWavetableEnv();



    
    
private:
    double sampleRate = 0;
    bool keyIsDown = false;
    
    double startPosition = 0;
    int currentMidiNumber = 0;
    int fluxMode = 0;
    int numToChange = 0;
    
    double pitchRatio = 0;
    double sourceSamplePosition = 0;
    double numPlayedSamples = 0;
    float lgain = 0, rgain = 0;

    //needs to be over the actual range - value of 1.0f will throw error when envShape value is really one
    float envShapeValue = 2.0f;
    
    

    juce::ADSR adsr;
    
    WavetableEnvelope envCurve;
    
//    JUCE_LEAK_DETECTOR (GrainVoice)
};


