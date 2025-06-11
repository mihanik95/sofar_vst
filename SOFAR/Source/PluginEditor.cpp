/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SOFARAudioProcessorEditor::SOFARAudioProcessorEditor (SOFARAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (400, 200);

    distanceSlider.setSliderStyle (juce::Slider::Rotary);
    distanceSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (distanceSlider);

    panSlider.setSliderStyle (juce::Slider::Rotary);
    panSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible (panSlider);

    distanceLabel.setText ("Distance", juce::dontSendNotification);
    distanceLabel.attachToComponent (&distanceSlider, false);

    panLabel.setText ("Pan", juce::dontSendNotification);
    panLabel.attachToComponent (&panSlider, false);

    distanceAttachment.reset (new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "distance", distanceSlider));
    panAttachment.reset (new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.parameters, "pan", panSlider));

    attachButton (buttonA, 0);
    attachButton (buttonB, 1);
    attachButton (buttonC, 2);
    attachButton (buttonD, 3);
}

SOFARAudioProcessorEditor::~SOFARAudioProcessorEditor()
{
}

//==============================================================================
void SOFARAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void SOFARAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    distanceSlider.setBounds (area.removeFromLeft (120));
    panSlider.setBounds (area.removeFromLeft (120));
    auto buttonArea = area.removeFromTop (30);
    buttonA.setBounds (buttonArea.removeFromLeft (40));
    buttonB.setBounds (buttonArea.removeFromLeft (40));
    buttonC.setBounds (buttonArea.removeFromLeft (40));
    buttonD.setBounds (buttonArea.removeFromLeft (40));
}

void SOFARAudioProcessorEditor::attachButton(juce::TextButton& button, int index)
{
    addAndMakeVisible (button);
    button.onClick = [this, index]
    {
        audioProcessor.parameters.getParameter("space")->setValueNotifyingHost ((float) index / 3.0f);
    };
}
