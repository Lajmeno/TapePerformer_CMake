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

    enum ColourIds
    {
        backgroundColourId          = 0x1001200,  /**< A colour to use to fill the slider's background. */
        thumbColourId               = 0x1001300,  /**< The colour to draw the thumb with. It's up to the look
                                                       and feel class how this is used. */
        trackColourId               = 0x1001310,  /**< The colour to draw the groove that the thumb moves along. */
        rotarySliderFillColourId    = 0x1001311,  /**< For rotary sliders, this colour fills the outer curve. */
        rotarySliderOutlineColourId = 0x1001312,  /**< For rotary sliders, this colour is used to draw the outer curve's outline. */

        textBoxTextColourId         = 0x1001400,  /**< The colour for the text in the text-editor box used for editing the value. */
        textBoxBackgroundColourId   = 0x1001500,  /**< The background colour for the text-editor box. */
        textBoxHighlightColourId    = 0x1001600,  /**< The text highlight colour for the text-editor box. */
        textBoxOutlineColourId      = 0x1001700   /**< The colour to use for a border around the text-editor box. */
    };

    CustomLookAndFeel() {
        setColour(juce::Slider::thumbColourId, juce::Colour::fromString("#EB5353"));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromString("#EB5353"));

        //setColour( juce::TextButton::textColourOnId, juce::Colours::grey);
        setColour( juce::TextButton::buttonColourId, juce::Colours::grey.darker(0.2f));
        setColour( juce::TextButton::buttonOnColourId, findColour(thumbColourId));


        setColour(backgroundColourId, juce::Colour::fromString("#187498").brighter(.3f));
        setColour(thumbColourId, juce::Colour::fromString("#EB5353"));
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


    void drawButtonText(juce::Graphics &g, juce::TextButton &button, bool, bool isButtonDown) override {
        auto font = getTextButtonFont(button, button.getHeight());
        g.setFont(font);
        g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOffId
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
                          Slider &slider) override
    {
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

    void drawTickBox (Graphics& g,
                     Component& /*component*/,
                     float x, float y, float w, float h,
                     const bool ticked,
                     const bool isEnabled,
                     const bool /*isMouseOverButton*/,
                     const bool isButtonDown)
    {
        Path box;
        box.addRoundedRectangle (0.0f, 2.0f, 6.0f, 6.0f, 1.0f);

        g.setColour (isEnabled ? Colours::blue.withAlpha (isButtonDown ? 0.3f : 0.1f)
                               : Colours::lightgrey.withAlpha (0.1f));

        AffineTransform trans (AffineTransform::scale (w / 9.0f, h / 9.0f).translated (x, y));

        g.fillPath (box, trans);

        g.setColour (Colours::black.withAlpha (0.6f));
        g.strokePath (box, PathStrokeType (2.0f), trans);

        if (ticked)
        {
            g.setColour (findColour (thumbColourId));

            Path p;
            p.addRoundedRectangle(0.8f, 2.8f, 4.5f, 4.5f, 0.5f);
            g.fillPath (p, trans);

            g.strokePath (p, PathStrokeType (0.3f), trans);
        }
    }


    void drawToggleButton (Graphics& g,
                                              ToggleButton& button,
                                              bool isMouseOverButton,
                                              bool isButtonDown)
    {

        const int tickWidth = jmin (20, button.getHeight());



        drawTickBox (g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f,
                     (float) tickWidth, (float) tickWidth,
                     button.getToggleState(),
                     button.isEnabled(),
                     isMouseOverButton,
                     isButtonDown);

        g.setColour (button.findColour (ToggleButton::textColourId));
        g.setFont (jmin (15.0f, button.getHeight() * 0.6f));

        if (! button.isEnabled())
            g.setOpacity (0.5f);

        const int textX = tickWidth + 5;

        g.drawFittedText (button.getButtonText(),
                          textX, 4,
                          button.getWidth() - textX - 2, button.getHeight() - 8,
                          Justification::centredLeft, 10);
    }



};