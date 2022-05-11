/*
  ==============================================================================

    WaveDisplay.h
    Created: 25 Jan 2022 6:52:40pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class WaveDisplay  : public juce::Component,
public juce::FileDragAndDropTarget,
private juce::Timer
//, public juce::ChangeListener
{
public:
    WaveDisplay(TapePerformerAudioProcessor&);
    ~WaveDisplay() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;
    
    void paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    

private:
    
    juce::TextButton mLoadButton { "Load" };
    
    void timerCallback() override
    {
        repaint();
    }
    
    
    TapePerformerAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveDisplay)
};
