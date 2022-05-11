/*
  ==============================================================================

    WaveDisplay.cpp
    Created: 25 Jan 2022 6:52:40pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WaveDisplay.h"

//==============================================================================
WaveDisplay::WaveDisplay(TapePerformerAudioProcessor& p) : audioProcessor(p)
{
    mLoadButton.onClick = [&]() { audioProcessor.loadFile(); };
    //    audioProcessor.transportSource.addChangeListener (this);
    addAndMakeVisible(mLoadButton);
    
//    audioProcessor.thumbnail.addChangeListener(this);
    
    startTimer(40);


}

WaveDisplay::~WaveDisplay()
{
}

void WaveDisplay::paint (juce::Graphics& g)
{
    
    g.fillAll(juce::Colours::darkgrey);
            
    auto bounds = getLocalBounds();
    auto waveFileArea = bounds.removeFromTop(bounds.getHeight()* 0.95);
    
    
    juce::Rectangle<int> thumbnailBounds (0, 0, waveFileArea.getWidth(), waveFileArea.getHeight());
    
    
    
    if (audioProcessor.thumbnail.getNumChannels() == 0)
    {
        paintIfNoFileLoaded (g, thumbnailBounds);
    }
    else
    {
        paintIfFileLoaded (g, thumbnailBounds);
    }
    
    mLoadButton.setBounds(waveFileArea.getWidth() * 0.95, waveFileArea.getHeight() * 0.02, 40, 20);
    
    auto waveArea = bounds.removeFromTop(bounds.getHeight());
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    
    if (audioProcessor.getNumSamplerSounds() < 0)
    {
        g.drawText("Drag and Drops a Sample", waveArea.getWidth() * 0.5, waveArea.getHeight() + waveArea.getHeight() * 0.5, 100, 20, juce::Justification::centred);
    }

}

void WaveDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

bool WaveDisplay::isInterestedInFileDrag(const juce::StringArray &files)
{
    for ( auto file:files)
    {
        if (file.contains(".wav")||file.contains(".mp3")||file.contains(".aif"))
        {
            return true;
        }
    }
    return false;
}


void WaveDisplay::filesDropped(const juce::StringArray &files, int x, int y)
{
    for ( auto file:files)
    {
        if (isInterestedInFileDrag(files))
        {
            audioProcessor.loadFile (file);
        }
    }
    repaint();
}

void WaveDisplay::paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    
    g.setColour (juce::Colours::darkorange);
    g.fillRect (thumbnailBounds);

    g.setColour (juce::Colours::lightseagreen);
    

    //make draw Wave only when new file is loaded ? or does it need to draw new when draw positio is drawn on top??
    
    audioProcessor.thumbnail.drawChannels (g, thumbnailBounds, 0.0, audioProcessor.thumbnail.getTotalLength(), 1.0f);
    

    auto audioLength = (float) audioProcessor.thumbnail.getTotalLength();
    
    for (int i = 0; i < audioProcessor.mNumVoices; i++)
    {
                                                      
        if (auto voice = dynamic_cast<GrainVoice*>(audioProcessor.mSampler.getVoice(i)))
        {
//            audioProcessor.wavePlayPosition[i] = voice->getPosition();
            auto audioPosition = voice->getPosition() /  audioProcessor.getSampleRate();

            auto drawPosition = (audioPosition / audioLength) * (float) thumbnailBounds.getWidth() + (float) thumbnailBounds.getX();

            if (auto sound = dynamic_cast<GrainSound*>(audioProcessor.mSampler.getSound(0).get()))
            {
                //get MidiNotenUmber that is trigerred if it is the root than draw red !!needs to be changed here!!!
                if ( audioProcessor.midiNoteForNormalPitch == 61 )
                {
                    g.setColour (juce::Colours::red);
                    g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition, (float) thumbnailBounds.getBottom(), 1.5f);
                }
                else
                {
                    g.setColour (juce::Colours::black);
                    g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition, (float) thumbnailBounds.getBottom(), 1.0f);
                }
            }
    
        }
    }
    
    
    if (auto sound = dynamic_cast<GrainSound*>(audioProcessor.mSampler.getSound(0).get()))
    {
        auto& numKeys = *audioProcessor.apvts.getRawParameterValue("numKeys");
        auto numFragments = numKeys ?  12 : 24;
        auto widthOfFragment = *audioProcessor.apvts.getRawParameterValue("duration") * audioLength;
        auto initialXPosition = *audioProcessor.apvts.getRawParameterValue("position") * audioLength;
        auto& spreadParam = *audioProcessor.apvts.getRawParameterValue("spread");
        
        
        for (int i = 0; i < numFragments; i++)
        {
            
            juce::Rectangle<int> fragmentBounds (initialXPosition / audioLength * thumbnailBounds.getWidth(), 0, widthOfFragment / audioLength * thumbnailBounds.getWidth(), thumbnailBounds.getHeight());

            initialXPosition = std::fmod(initialXPosition + (audioLength / numFragments) * spreadParam, audioLength);
            
            auto purpleHue = juce::Colours::royalblue.getHue();
            g.setColour (juce::Colour::fromHSV (purpleHue, .2f, 1.f, 0.4f));
            g.fillRect (fragmentBounds);
            
            if( initialXPosition + widthOfFragment > audioLength)
            {
                juce::Rectangle<int> fragmentBounds (0, 0, ( (initialXPosition + widthOfFragment) - audioLength) / audioLength * thumbnailBounds.getWidth(), thumbnailBounds.getHeight());
                
//                auto purpleHue = juce::Colours::darkgrey.getHue();
//                g.setColour (juce::Colour::fromHSV (purpleHue, 0.3f, 0.5f, 0.25f));
                g.fillRect (fragmentBounds);
                
            }
        }
    }


}

void WaveDisplay::paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::black);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("Drag and Drops a File here", thumbnailBounds, juce::Justification::centred, 1);
}
