//
//  AnimatedToggleSlider.h
//  TapePerformer
//
//  Created by Abdullah Ismailogullari on 06.06.22.
//
#pragma once

#include <JuceHeader.h>

class CustomToggleButton : public Button, Timer
{
public:
    CustomToggleButton(const String &textL, const String &textR) : Button({}), textL(textL), textR(textR) { setClickingTogglesState(true); }

private:
    void buttonStateChanged () override { startTimer(30); }

    void timerCallback() override
    {
        auto rate = 0.25f;
        rate *= getToggleState() ? 1.0f : -1.0f;

        position = jlimit(0.0f, 1.0f, position + rate);

        if (position == 0.0f || position == 1.0f)
            stopTimer();

        repaint();
    }
private:

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        auto h = float(getWidth() * 0.5);
        auto fullArea = getLocalBounds().toFloat();
        auto area = getLocalBounds().toFloat();
        
        g.setFont(juce::Font(12.0f, juce::Font::bold));
        
        
        
        //auto bgColour = juce::Colour::fromString("#EB5353").getHue();
        //auto backgroundColour = juce::Colour(bgColour, .6f, 0.2f, 0.1f);
        auto backgroundColour = juce::Colours::grey;
        //auto highlightedBackgroundColour = juce::Colour(bgColour, .9f, 1.f, 0.8f);
        auto highlightedBackgroundColour = juce::Colour::fromString("#EB5353");
        auto chosenTextColour = juce::Colours::white;
        
        g.setColour(backgroundColour);
        g.fillRect(area.reduced(1.5f));

        Path p;  // leaving as a path so an optional outline can be added
        auto areaL = area.removeFromLeft(area.getWidth() * 0.5f);
        
        
        
        if(!getToggleState())
        {
            
            p.addRectangle(areaL.reduced(2.0f));
            g.setColour(highlightedBackgroundColour);
            g.fillPath(p);
            g.setColour(chosenTextColour);
            g.drawText(textL, areaL, juce::Justification::centred, true);
            g.setColour(Colours::lightgrey);
            g.drawText(textR, area, juce::Justification::centred, true);
        }
        else
        {
            p.addRectangle(area.reduced(2.0f));
            g.setColour(highlightedBackgroundColour);
            g.fillPath(p);
            g.setColour(Colours::lightgrey);
            g.drawText(textL, areaL, juce::Justification::centred, true);
            g.setColour(chosenTextColour);
            g.drawText(textR, area, juce::Justification::centred, true);
        }
        //area = areaL + area;
        auto circleBounds = fullArea.withWidth(h).translated((fullArea.getWidth() - h) * position, 0.0f);
        g.setColour(Colours::black);
        g.drawRoundedRectangle(fullArea, 4, 1.9f);
        g.drawRoundedRectangle(circleBounds, 4, 3.0f);
    }

    float position{ 0.0f };

    const String textL;
    const String textR;
    
};



