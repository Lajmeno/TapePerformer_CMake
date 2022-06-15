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

void GrainSound::updateParams(float mode, float availableKeys, double position, double duration, float spread, std::vector<float> fluxMode)
{
    pitchModeParam = mode >= 1;
    
    numOfKeysAvailable = (availableKeys == 1) ? 24 : 48;
    
    positionParam = position * length;
    
    // change here to a state that won't increase much if a sample is very long
//    auto lengthInSeconds = length / sourceSampleRate;
//    lengthInSeconds > 3 ? durationParam = duration * ( 2.5 * sourceSampleRate) : durationParam = duration * length;
    if(length > 88200)
        duration *= 88200.0f / length;

    durationParam = std::max(duration * length, 40.0);

    spreadParam = spread;

    int counter = 1;
    fluxModeParam = 0;
    for(auto mode : fluxMode)
    {
        if(mode > 0)
        {
            fluxModeParam = counter;
        }
        counter++;
    }
    
}

//==============================================================================
GrainVoice::GrainVoice() : envCurve()  //: createWavetableEnv(), envCurve(envTable) {
{

    std::srand(time(NULL));
    
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
        currentMidiNumber = midiNoteNumber;
        numToChange = 0;
        
        if(sound->pitchModeParam)
        {
            pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0) * sound->sourceSampleRate / getSampleRate();
            sourceSamplePosition = setStartPosition(sound, true);
        }
        else
        {
            pitchRatio = std::pow (2.0, (sound->transpositionParam - sound->midiRootNote) / 12.0) * sound->sourceSampleRate / getSampleRate();
            sourceSamplePosition = setStartPosition(sound, true);
        }

        numPlayedSamples = 0;
        lgain = velocity;
        rgain = velocity;

        adsr.setSampleRate (sound->sourceSampleRate);
        adsr.setParameters (sound->params);

        adsr.noteOn();
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
            float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha) : l;

            auto envelopeValue = adsr.getNextSample();
            auto envTableValue = envCurve.getNextSample();

            l *= lgain * envTableValue * envelopeValue;
            r *= rgain * envTableValue * envelopeValue;

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
                //need to make this only be called once (maybe once a block) - at the moment it is called for every sample in the block
                stopNote (0.0f, true);
            }
            else if (numPlayedSamples > playingSound->durationParam)
            {
                numPlayedSamples = 0;
                sourceSamplePosition = setStartPosition(playingSound, false);
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

double GrainVoice::setStartPosition(GrainSound* sound, bool newlyStarted)
{
    envCurve.resetIndex();
    setEnvelopeFrequency(sound);
    if(!newlyStarted)
    {
        switch (sound->fluxModeParam)
        {
            case 1 : case 2 :
                numToChange = (numToChange + 1) % sound->numOfKeysAvailable ;
                break;
            case 3 :
                if(numToChange <= 0)
                {
                    numToChange *= -1;
                    numToChange = (numToChange + 1) % (sound->numOfKeysAvailable / 2);
                }
                else
                {
                    numToChange *= -1;
                }
                break;
            case 4 :
                numToChange = std::rand() % sound->numOfKeysAvailable;



        }
    }
    double position;
    if(!sound->pitchModeParam)
    {
        if(sound->fluxModeParam == 2)
        {
            position = std::fmod((sound->positionParam +  (float((currentMidiNumber - numToChange) % sound->numOfKeysAvailable) / float(sound->numOfKeysAvailable) * sound->length) * sound->spreadParam), sound->length);
        }
        else
        {
            position = std::fmod((sound->positionParam +  (float((currentMidiNumber + numToChange)   % sound->numOfKeysAvailable) / float(sound->numOfKeysAvailable) * sound->length) * sound->spreadParam), sound->length);
        }
    }
    else
    {
        setEnvelopeFrequency(sound);
        envCurve.resetIndex();
        if(sound->fluxModeParam == 2)
        {
            position = std::fmod((sound->positionParam +  (float((sound->midiRootNote - numToChange) % sound->numOfKeysAvailable) / float(sound->numOfKeysAvailable) * sound->length) * sound->spreadParam), sound->length);
        }
        else
        {
            position = std::fmod((sound->positionParam +  (float((sound->midiRootNote + numToChange) % sound->numOfKeysAvailable) / float(sound->numOfKeysAvailable) * sound->length) * sound->spreadParam), sound->length);
        }

    }
    return position;
}

void GrainVoice::setEnvelopeFrequency(GrainSound* sound)
{
    auto frequency = 1 / ( (sound->durationParam / pitchRatio) / getSampleRate());
    envCurve.setFrequency ((float) frequency, getSampleRate());
}



