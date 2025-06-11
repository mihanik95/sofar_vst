/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SOFARAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SOFARAudioProcessorEditor (SOFARAudioProcessor&);
    ~SOFARAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SOFARAudioProcessor& audioProcessor;
    juce::Slider distanceSlider;
    juce::Label distanceLabel;
    juce::Slider panSlider;
    juce::Label panLabel;
    juce::TextButton buttonA { "A" };
    juce::TextButton buttonB { "B" };
    juce::TextButton buttonC { "C" };
    juce::TextButton buttonD { "D" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttachment;
    void attachButton(juce::TextButton& button, int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SOFARAudioProcessorEditor)
};
