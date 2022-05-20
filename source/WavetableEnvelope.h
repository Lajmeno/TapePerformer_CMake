/*
  ==============================================================================

    Wavetable.h
    Created: 28 Jan 2022 11:07:11am
    Author:  Abdullah Ismailogullari

  ==============================================================================
 From:
 
 
  name:             WavetableSynthTutorial
 version:          3.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Wavetable synthesiser.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_processors, juce_audio_utils, juce_core,
                   juce_data_structures, juce_events, juce_graphics,
                   juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             Component
 mainClass:        MainContentComponent

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

#include <JuceHeader.h>

//==============================================================================
class WavetableEnvelope
{
public:
    WavetableEnvelope ()
    {
    }

    void setFrequency (float frequency, float sampleRate)
    {
        auto newTableSize = (unsigned int) wavetable.getNumSamples() - 1;
        auto tableSizeOverSampleRate = (float) newTableSize / sampleRate;
        tableDelta = frequency * tableSizeOverSampleRate;
    }

    forcedinline float getNextSample() noexcept
    {
        auto newTableSize = wavetable.getNumSamples() - 1;
        auto index0 = (unsigned int) currentIndex;
        auto index1 = index0 + 1;

        auto frac = currentIndex - (float) index0;

        auto* table = wavetable.getReadPointer (0);
        auto value0 = table[index0];
        auto value1 = table[index1];

        auto currentSample = value0 + frac * (value1 - value0);

        if ((currentIndex += tableDelta) > (float) newTableSize)
            currentIndex -= (float) newTableSize;

        return currentSample;
    }
    
    void createWavetableEnv()
    {
        wavetable.setSize (1, (int) tableSize + 1);
        auto* samples = wavetable.getWritePointer (0);

        float envShapeParam = envelopeShape * 6.8f;

        //auto angleDelta = juce::MathConstants<double>::twoPi / (double) (tableSize - 1);
        //auto currentAngle = 0.0;

        for (unsigned int i = 0; i < tableSize; ++i)
        {

            float x = (i - 1024.0f) / 512.0f ;
            float sample = std::tanh(std::exp(-x*x) * envShapeParam);
            samples[i] = std::min(sample * envShapeParam, 1.0f);
            /*
            auto sample = std::sin (currentAngle);
            samples[i] = std::min((float) sample * envShapeParam, 1.0f);
            currentAngle += angleDelta;
             */
        }

        samples[tableSize] = samples[0];
    }

    void resetIndex()
    {
        currentIndex = 0.0f;
    }
    
    juce::AudioSampleBuffer getWavetable (){
        return wavetable;
    }

    static float envelopeShape;


private:
    juce::AudioSampleBuffer wavetable;



    const unsigned int tableSize = 1 << 11;
    float currentIndex = 0.0f, tableDelta = 0.0f;
};
