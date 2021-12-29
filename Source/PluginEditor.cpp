/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalEQAudioProcessorEditor::VocalEQAudioProcessorEditor (VocalEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
        highPassFilterAttachment(*p.highPassFilter, highPassFilterSlider),
        trebleBoostAttachment(*p.trebleBoost, trebleBoostSlider),
        rollOffAttachment(*p.rollOff, rollOffSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 280);
    
    addAndMakeVisible (highPassFilterSlider);
    highPassFilterSlider.setTextValueSuffix (" Hz");
    highPassFilterSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    highPassFilterSlider.setColour(juce::Slider::ColourIds::thumbColourId, yellow);
    highPassFilterSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    highPassFilterSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    highPassFilterSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    highPassFilterSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, yellow);
    highPassFilterSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (trebleBoostSlider);
    trebleBoostSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    trebleBoostSlider.setColour(juce::Slider::ColourIds::thumbColourId, yellow);
    trebleBoostSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    trebleBoostSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    trebleBoostSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    trebleBoostSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, yellow);
    trebleBoostSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (rollOffSlider);
    rollOffSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    rollOffSlider.setColour(juce::Slider::ColourIds::thumbColourId, yellow);
    rollOffSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    rollOffSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    rollOffSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    rollOffSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, yellow);
    rollOffSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
}

VocalEQAudioProcessorEditor::~VocalEQAudioProcessorEditor()
{
}

//==============================================================================
void VocalEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (black);
    g.setColour(white);
    g.setFont(18);
    g.drawFittedText("Vocal EQ", 40,  30, 320, 30, juce::Justification::left, 1);
    
    g.setColour(yellow);
    g.setFont(15);
    g.drawFittedText("High pass filter", 40,  90, 100, 30, juce::Justification::left, 1);
    
    g.setColour(yellow);
    g.setFont(15);
    g.drawFittedText("Treble boost", 40, 130, 100, 30, juce::Justification::left, 1);
    
    g.setColour(yellow);
    g.setFont(15);
    g.drawFittedText("Roll off", 40, 170, 100, 30, juce::Justification::left, 1);
    
    g.setColour(grey);
    g.setFont(12);
    g.drawFittedText("Unusual Audio", 40, getHeight() - 60, 320, 30, juce::Justification::left, 1);
}

void VocalEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    highPassFilterSlider.setBounds (140,  95, getWidth() - 140 - 40, 20);
    trebleBoostSlider   .setBounds (140, 135, getWidth() - 140 - 40, 20);
    rollOffSlider       .setBounds (140, 175, getWidth() - 140 - 40, 20);
}
