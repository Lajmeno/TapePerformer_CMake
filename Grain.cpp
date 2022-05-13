/*
  ==============================================================================

    Grain.cpp
    Created: 7 Jan 2022 11:10:46pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/
#include "Grain.h"



GrainSound::GrainSound (const juce::String& soundName,
                            juce::AudioFormatReader& source,
                            const juce::BigInteger& notes,
                            int midiNoteForNormalPitch,
                            double attackTimeSecs,
                            double releaseTimeSecs,
                            double maxSampleLengthSeconds)
    : name (soundName),
      sourceSampleRate (source.sampleRate),
      midiNotes (notes),
      midiRootNote (midiNoteForNormalPitch)
{
    if (sourceSampleRate > 0 && source.lengthInSamples > 0)
    {
        length = juce::jmin ((int) source.lengthInSamples,
                       (int) (maxSampleLengthSeconds * sourceSampleRate));

        data.reset (new juce::AudioBuffer<float> (juce::jmin (2, (int) source.numChannels), length + 4));

        source.read (data.get(), 0, length + 4, 0, true, true);

        params.attack  = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);
    }
}

GrainSound::~GrainSound()
{
}

bool GrainSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes[midiNoteNumber];
}

bool GrainSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

void GrainSound::updateParams(float mode, float availableKeys, double position, double duration, float spread)
{
    pitchModeParam = mode < 1;
    
    numOfKeysAvailable = (availableKeys == 1) ? 12 : 24;
    
    positionParam = position * length;
    
    // change here to a state that won't increase much if a sample is very long
//    auto lengthInSeconds = length / sourceSampleRate;
//    lengthInSeconds > 3 ? durationParam = duration * ( 2.5 * sourceSampleRate) : durationParam = duration * length;
    durationParam = duration * length;
    
    spreadParam = spread;
    
}

//==============================================================================
GrainVoice::GrainVoice() : envCurve()  //: createWavetableEnv(), envCurve(envTable) {
{

//    envCurve = new WavetableEnvelope(envTable);
//    envCurve.add(envCurves);
    
}
GrainVoice::~GrainVoice() {}

bool GrainVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<const GrainSound*> (sound) != nullptr;
}

void GrainVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* s, int /*currentPitchWheelPosition*/)
{
    if(envShapeValue != WavetableEnvelope::envelopeShape) {
        envCurve.createWavetableEnv();
        envShapeValue = WavetableEnvelope::envelopeShape;
    }

    if (auto* sound = dynamic_cast< GrainSound*> (s)) //deleted const before GrainSound* to make set startPosition work
    {
        currentMiniNumber = midiNoteNumber;
        
        if(sound->pitchModeParam)
        {
            
            pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0) * sound->sourceSampleRate / getSampleRate();
            
            sourceSamplePosition = sound->positionParam;
        }
        else
        {
            pitchRatio = std::pow (2.0, (sound->transpositionParam - sound->midiRootNote) / 12.0) * sound->sourceSampleRate / getSampleRate();
            
//            sourceSamplePosition = std::fmod((sound->positionParam +  (float(midiNoteNumber % sound->numOfKeysAvailable) / float(sound->numOfKeysAvailable) * sound->length) * sound->spreadParam), sound->length);
            sourceSamplePosition = setStartPosition(sound, midiNoteNumber);
            
        }
        startPosition = sourceSamplePosition;

        numPlayedSamples = 0;
        lgain = velocity;
        rgain = velocity;

        adsr.setSampleRate (sound->sourceSampleRate);
        adsr.setParameters (sound->params);

        adsr.noteOn();
        

        auto frequency = 1 / (sound->getDurationParam() / getSampleRate());
        envCurve.setFrequency ((float) frequency, getSampleRate());
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void GrainVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        adsr.reset();
    }
}

void GrainVoice::pitchWheelMoved (int /*newValue*/) {}
void GrainVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

//==============================================================================
void GrainVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (auto* playingSound = static_cast<GrainSound*> (getCurrentlyPlayingSound().get()))
    {
        auto& data = *playingSound->data;
        const float* const inL = data.getReadPointer (0);
        const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer (1) : nullptr;

        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        

        while (--numSamples >= 0)
        {
            auto pos = (int) sourceSamplePosition;
            auto alpha = (float) (sourceSamplePosition - pos);
            auto invAlpha = 1.0f - alpha;

            // just using a very simple linear interpolation here..
            float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha)
                                       : l;

            auto envelopeValue = adsr.getNextSample();
            auto envTableValue = envCurve.getNextSample();

            l *= lgain * envelopeValue * envTableValue;
            r *= rgain * envelopeValue * envTableValue;

            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }

            sourceSamplePosition += pitchRatio;
            sourceSamplePosition = std::fmod(sourceSamplePosition, playingSound->length);
            
            numPlayedSamples += pitchRatio;
            
            if (!isKeyDown())
            {
                stopNote (0.0f, false);
                break;
            }
            else if (numPlayedSamples > playingSound->durationParam)
            {
                numPlayedSamples = 0;
                if(!playingSound->pitchModeParam)
                    sourceSamplePosition = setStartPosition(playingSound, currentMiniNumber);
                else
                    sourceSamplePosition = startPosition;
            }

        }
    }
    
}

//==============================================================================

double GrainVoice::getPosition()
{
    double position;
    (!isKeyDown()) ? (position = 0) : (position = sourceSamplePosition);
    return position;
}

double GrainVoice::setStartPosition(GrainSound* sound, int midiNoteNumber)
{
    auto position = std::fmod((sound->positionParam +  (float(midiNoteNumber % sound->numOfKeysAvailable) / float(sound->numOfKeysAvailable) * sound->length) * sound->spreadParam), sound->length);
    return position;
}



