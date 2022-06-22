/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TapePerformerAudioProcessor::TapePerformerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
,thumbnailCache (2),                            // [4]
thumbnail (512, mFormatManager, thumbnailCache),
apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    
    modeParameter = apvts.getRawParameterValue ("playMode");
    availableKeysParameter = apvts.getRawParameterValue("numKeys");
    firstFluxParameter = apvts.getRawParameterValue("firstFluxMode");
    secondFluxParameter = apvts.getRawParameterValue("secondFluxMode");
    thirdFluxParameter = apvts.getRawParameterValue("thirdFluxMode");
    fourthFluxParameter = apvts.getRawParameterValue("fourthFluxMode");
    positionParameter = apvts.getRawParameterValue("position");
    durationParameter = apvts.getRawParameterValue("duration");
    spreadParameter = apvts.getRawParameterValue("spread");
    gainParameter  = apvts.getRawParameterValue ("gain");
    envelopeShapeParameter = apvts.getRawParameterValue("envShape");
    
    
    mFormatManager.registerBasicFormats();

    
    for (int i = 0; i < mNumVoices; i++)
    {
        mSampler.addVoice(new GrainVoice());
    }
}
 
TapePerformerAudioProcessor::~TapePerformerAudioProcessor()
{
    mFormatReader = nullptr;
}

//==============================================================================
const juce::String TapePerformerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TapePerformerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TapePerformerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TapePerformerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TapePerformerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TapePerformerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TapePerformerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TapePerformerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TapePerformerAudioProcessor::getProgramName (int index)
{
    return {};
}

void TapePerformerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TapePerformerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mSampler.setCurrentPlaybackSampleRate(sampleRate);
    
    previousGain = *gainParameter;
    
}

void TapePerformerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TapePerformerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TapePerformerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    WavetableEnvelope::envelopeShape = *apvts.getRawParameterValue("envShape");
    
    if (auto sound = dynamic_cast<GrainSound*>(mSampler.getSound(0).get()))
    {
        auto& mode = *apvts.getRawParameterValue("playMode");
        auto& availableKeys = *apvts.getRawParameterValue("numKeys");
        auto& position = *apvts.getRawParameterValue("position");
        auto& duration = *apvts.getRawParameterValue("duration");
        auto& spread = *apvts.getRawParameterValue("spread");
        std::vector<float> fluxMode = {*apvts.getRawParameterValue("firstFluxMode"), *apvts.getRawParameterValue("secondFluxMode"), *apvts.getRawParameterValue("thirdFluxMode"), *apvts.getRawParameterValue("fourthFluxMode")};
        sound->updateParams(mode, (int)availableKeys, (double)position, (double)duration, spread, fluxMode);
        
    }
    
    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    
    //Smooth Gain Multiplication with Ramp
    auto& currentGain = *gainParameter;
    if (currentGain == previousGain)
    {
        buffer.applyGain (currentGain);
    }
    else
    {
        buffer.applyGainRamp (0, buffer.getNumSamples(), previousGain, currentGain);

        previousGain = currentGain;
    }
    
}

//==============================================================================
bool TapePerformerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TapePerformerAudioProcessor::createEditor()
{
    return new TapePerformerAudioProcessorEditor (*this);
}

//==============================================================================
void TapePerformerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void TapePerformerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
 
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}


void TapePerformerAudioProcessor::loadFile()
{
    
    
    //bug: when closing Filechooser without choosing a file - no file is load
    //need to keep the old file in that situation
    
    chooser = std::make_unique<juce::FileChooser> ("Select a Wave file shorter than 10 minutes to play...",
                                                   juce::File{},
                                                   "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode
                      | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
    {
        auto file = fc.getResult();

//        if (file == juce::File{})
//            return;
        if (file != juce::File{})
        {
            std::unique_ptr<juce::AudioFormatReader> reader (mFormatManager.createReaderFor (file)); // [2]
    //        mFormatReader = mFormatManager.createReaderFor(file);
     
            if (reader.get() != nullptr)
            {
                mSampler.clearSounds();
                mFormatReader = mFormatManager.createReaderFor(file);
                thumbnail.setSource (new juce::FileInputSource (file));
                
                juce::BigInteger range;
                range.setRange(0, 127, true);
                mSampler.addSound(new GrainSound("Sample", *mFormatReader, range, midiNoteForNormalPitch, 0.0f, 0.01f, 180));
            }
        }
    });
}
 
 
void TapePerformerAudioProcessor::loadFile(const juce::String &path)
{
    mSampler.clearSounds();
    auto file = juce::File (path);
    mFormatReader = mFormatManager.createReaderFor(file);
    thumbnail.setSource (new juce::FileInputSource (file));
    
    juce::BigInteger range;
    range.setRange(0, 127, true);
    
    mSampler.addSound(new GrainSound("Sample", *mFormatReader, range, midiNoteForNormalPitch, 0.0f, 0.01f, 180));
    
//    wavePlayPosition = 0;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapePerformerAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout TapePerformerAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    
    params.add(std::make_unique<juce::AudioParameterChoice>("playMode", "Playing Mode", juce::StringArray("Position Mode", "Pitch Mode"), 1));
    
    params.add(std::make_unique<juce::AudioParameterChoice>("numKeys", "Keys Available", juce::StringArray("12 Keys", "24 Keys", "48 Keys", "96 Keys"), 0));

    params.add(std::make_unique<juce::AudioParameterBool>("firstFluxMode", "First Flux Mode", false));

    params.add(std::make_unique<juce::AudioParameterBool>("secondFluxMode", "Second Flux Mode", false));

    params.add(std::make_unique<juce::AudioParameterBool>("thirdFluxMode", "Third Flux Mode", false));

    params.add(std::make_unique<juce::AudioParameterBool>("fourthFluxMode", "Fourth Flux Mode", false));

    params.add(std::make_unique<juce::AudioParameterFloat>("position", "SamplePosition", juce::NormalisableRange<float>(0.f, 1.f, 0.001f, 1.f), 0.25f));
    
    params.add(std::make_unique<juce::AudioParameterFloat>("duration", "Duration", juce::NormalisableRange<float>(0.f, 1.f, 0.001f, .4f), 0.15f));
    
    params.add(std::make_unique<juce::AudioParameterFloat>("spread", "Spread", juce::NormalisableRange<float>(0.f, 1.f, 0.001f, 0.4f), 1.f));
    
    params.add(std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0f, 1.0f, 0.7f));
    
    params.add(std::make_unique<juce::AudioParameterFloat>("envShape", "Shape", juce::NormalisableRange<float>(0.f, 1.f, 0.001f, 1.f), 0.0f));
        
    return params;

}

//this initializes a static variable of WavetableEnvelope - needs to be done in a cpp file
//this enables to use one global parameter for every instance of WavetableEnvelope - as the shape should be the same for every voice and for EnvelopeDisplay too
float WavetableEnvelope::envelopeShape{1};

