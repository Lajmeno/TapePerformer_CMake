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
    CustomToggleButton () : Button({}) { setClickingTogglesState(true); }

private:
    void buttonStateChanged () override { startTimer(30); }

    void timerCallback() override
    {
        
        /*
        auto rate = 0.1f;
        rate *= getToggleState() ? 1.0f : -1.0f;

        position = jlimit(0.0f, 1.0f, position + rate);

        if (position == 0.0f || position == 1.0f)
            stopTimer();
         */

        repaint();
    }

    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown) override
    {
        auto h = float(getHeight());
        auto area = getLocalBounds().toFloat();
        
        g.setFont(juce::Font(14.0f, juce::Font::plain));
        
        
        
        auto bgColour = juce::Colour::fromString("#f9ce4d").getHue();
        auto backgroundColour = juce::Colour(bgColour, .6f, 0.2f, 0.1f);
        auto highlightedBackgroundColour = juce::Colour(bgColour, .9f, 1.f, 0.8f);
        auto chosenTextColour = juce::Colours::white;
        
        g.setColour(highlightedBackgroundColour);
        g.fillRect(area.reduced(1.5f));

        Path p;  // leaving as a path so an optional outline can be added
        auto areaL = area.removeFromLeft(area.getWidth() * 0.5f);
        
        
        
        if(!getToggleState())
        {
            
            p.addRectangle(areaL.reduced(2.0f));
            g.setColour(highlightedBackgroundColour);
            g.fillPath(p);
            g.setColour(chosenTextColour);
            const String textL = "Position";
            g.drawText(textL, areaL, juce::Justification::centred, true);
            g.setColour(Colours::lightgrey);
            const String textR = "Pitch";
            g.drawText(textR, area, juce::Justification::centred, true);
            
            
            /*
            g.setColour(Colours::lightgrey);
            auto circleBounds = areaL.withWidth(h).translated((areaL.getWidth() - h) * position, 0.0f);
            g.fillEllipse(circleBounds.reduced(1.0f));
             */
        }
        else
        {
            p.addRectangle(area.reduced(2.0f));
            g.setColour(highlightedBackgroundColour);
            g.fillPath(p);
            g.setColour(Colours::lightgrey);
            const String textL = "Position";
            g.drawText(textL, areaL, juce::Justification::centred, true);
            g.setColour(chosenTextColour);
            const String textR = "Pitch";
            g.drawText(textR, area, juce::Justification::centred, true);
            
            
            /*
            
            g.setColour(Colours::lightgrey);
            auto circleBounds = area.withWidth(h).translated((area.getWidth() - h) * position, 0.0f);
            g.fillEllipse(circleBounds.reduced(1.0f));
             */
        }
        //area = areaL + area;
        g.setColour(Colours::black);
        g.drawRoundedRectangle(area, 8, 1.5f);
    }

    float position{ 0.50f };
    
};



