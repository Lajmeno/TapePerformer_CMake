//
// Created by Abdullah Ismailogullari on 10.06.22.
//

/*
  ==============================================================================

   This file is part of the JUCE tutorials.
   Copyright (c) 2020 - Raw Material Software Limited

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
   WHETHER EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR
   PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/


#pragma once

#include <JuceHeader.h>

//==============================================================================
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel() {
        setColour(juce::Slider::thumbColourId, juce::Colour::fromString("#EB5353"));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromString("#EB5353"));
        setColour(juce::Slider::backgroundColourId, juce::Colour::fromString("#187498").brighter(.3f));
    }

    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &slider) override {
        auto radius =  (float) juce::jmin(width / 2.15f, height / 2.15f) - 4.0f;
        auto centreX = (float) x + (float) width * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour(slider.findColour(Slider::rotarySliderFillColourId));
        g.fillEllipse(rx, ry, rw, rw);

        g.setColour(juce::Colours::black);
        g.fillEllipse(centreX - 2.0f, y, radius * 0.125f, radius * 0.125f);

        // outline
        g.setColour(juce::Colours::red);
        g.drawEllipse(rx, ry, rw, rw, 1.0f);


        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

        // pointer
        g.setColour(juce::Colours::white);
        g.fillPath(p);
    }

    void drawButtonBackground(juce::Graphics &g, juce::Button &button, const juce::Colour &backgroundColour,
                              bool, bool isButtonDown) override {
        auto buttonArea = button.getLocalBounds();
        auto edge = 4;

        buttonArea.removeFromLeft(edge);
        buttonArea.removeFromTop(edge);

        // shadow
        g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
        g.fillRect(buttonArea);

        auto offset = isButtonDown ? -edge / 2 : -edge;
        buttonArea.translate(offset, offset);

        g.setColour(backgroundColour);
        g.fillRect(buttonArea);
    }

    void drawButtonText(juce::Graphics &g, juce::TextButton &button, bool, bool isButtonDown) override {
        auto font = getTextButtonFont(button, button.getHeight());
        g.setFont(font);
        g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                              : juce::TextButton::textColourOffId)
                            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

        auto yIndent = juce::jmin(4, button.proportionOfHeight(0.3f));
        auto cornerSize = juce::jmin(button.getHeight(), button.getWidth()) / 2;

        auto fontHeight = juce::roundToInt(font.getHeight() * 0.6f);
        auto leftIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        auto rightIndent = juce::jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        auto textWidth = button.getWidth() - leftIndent - rightIndent;

        auto edge = 4;
        auto offset = isButtonDown ? edge / 2 : 0;

        if (textWidth > 0)
            g.drawFittedText(button.getButtonText(),
                             leftIndent + offset, yIndent + offset, textWidth, button.getHeight() - yIndent * 2 - edge,
                             juce::Justification::centred, 2);
    }

    void drawLinearSlider(Graphics &g,
                          int x, int y,
                          int w, int h,
                          float sliderPos,
                          float minSliderPos,
                          float maxSliderPos,
                          const Slider::SliderStyle style,
                          Slider &slider) override {
        //g.fillAll(slider.findColour(Slider::backgroundColourId));

        if (style == Slider::LinearBar)
        {
            g.setColour (slider.findColour (Slider::thumbColourId));
            g.fillRect (x, y, (int) sliderPos - x, h);

            g.setColour (slider.findColour (Slider::textBoxTextColourId).withMultipliedAlpha (0.5f));
            g.drawRect (x, y, (int) sliderPos - x, h);
        }
        else
        {
            g.setColour (slider.findColour (Slider::trackColourId)
                                 .withMultipliedAlpha (slider.isEnabled() ? 0.5f : 0.3f));

            if (slider.isHorizontal())
            {
                g.fillRect (x, y + roundToInt (h * 0.4f),
                            w, roundToInt (h * 0.1f));
            }
            else
            {
                g.fillRect (x + roundToInt (w * 0.5f - jmin (3.0f, w * 0.1f)), y,
                            jmin (4, roundToInt (w * 0.2f)), h);
            }

            float alpha = 0.35f;

            if (slider.isEnabled())
                alpha = slider.isMouseOverOrDragging() ? 1.0f : 0.7f;

            const Colour fill (slider.findColour (Slider::thumbColourId).withAlpha (alpha));
            const Colour outline (Colours::black.withAlpha (slider.isEnabled() ? 0.7f : 0.35f));

            if (style == Slider::TwoValueVertical || style == Slider::ThreeValueVertical)
            {
                drawTriangle (g, x + w * 0.5f + jmin (4.0f, w * 0.3f), minSliderPos,
                              x + w * 0.5f - jmin (8.0f, w * 0.4f), minSliderPos - 7.0f,
                              x + w * 0.5f - jmin (8.0f, w * 0.4f), minSliderPos,
                              fill, outline);

                drawTriangle (g, x + w * 0.5f + jmin (4.0f, w * 0.3f), maxSliderPos,
                              x + w * 0.5f - jmin (8.0f, w * 0.4f), maxSliderPos,
                              x + w * 0.5f - jmin (8.0f, w * 0.4f), maxSliderPos + 7.0f,
                              fill, outline);
            }
            else if (style == Slider::TwoValueHorizontal || style == Slider::ThreeValueHorizontal)
            {
                drawTriangle (g, minSliderPos, y + h * 0.6f - jmin (4.0f, h * 0.3f),
                              minSliderPos - 7.0f, y + h * 0.9f ,
                              minSliderPos, y + h * 0.9f,
                              fill, outline);

                drawTriangle (g, maxSliderPos, y + h * 0.6f - jmin (4.0f, h * 0.3f),
                              maxSliderPos, y + h * 0.9f,
                              maxSliderPos + 7.0f, y + h * 0.9f,
                              fill, outline);
            }

            if (style == Slider::LinearHorizontal || style == Slider::ThreeValueHorizontal)
            {
                /*
                drawTriangle (g, sliderPos, y + h * 0.9f,
                              sliderPos - 7.0f, y + h * 0.2f,
                              sliderPos + 7.0f, y + h * 0.2f,
                              fill, outline);
                              */
                g.setColour (slider.findColour (Slider::thumbColourId));
                g.drawLine (sliderPos, y + h * 0.25f, sliderPos, y + h * 0.65f, 3.0f);
            }
            else if (style == Slider::LinearVertical || style == Slider::ThreeValueVertical)
            {
                drawTriangle (g, x + w * 0.5f - jmin (4.0f, w * 0.3f), sliderPos,
                              x + w * 0.5f + jmin (8.0f, w * 0.4f), sliderPos - 7.0f,
                              x + w * 0.5f + jmin (8.0f, w * 0.4f), sliderPos + 7.0f,
                              fill, outline);
            }
        }
    }

    static void drawTriangle (Graphics& g, float x1, float y1, float x2, float y2, float x3, float y3, const Colour& fill, const Colour& outline) noexcept
    {
        Path p;
        p.addTriangle (x1, y1, x2, y2, x3, y3);
        g.setColour (fill);
        g.fillPath (p);

        g.setColour (outline);
        g.strokePath (p, PathStrokeType (0.3f));
    }


};