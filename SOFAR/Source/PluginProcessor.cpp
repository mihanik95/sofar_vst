/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SOFARAudioProcessor::SOFARAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters (*this, nullptr, "PARAMS", createParameters())
#endif
{
    updateIR();
}

SOFARAudioProcessor::~SOFARAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SOFARAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back (std::make_unique<juce::AudioParameterFloat> ("distance", "Distance", 0.0f, 1.0f, 0.0f));
    juce::StringArray choices { "A", "B", "C", "D" };
    params.push_back (std::make_unique<juce::AudioParameterChoice> ("space", "Space", choices, 0));
    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String SOFARAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SOFARAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SOFARAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SOFARAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SOFARAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SOFARAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SOFARAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SOFARAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SOFARAudioProcessor::getProgramName (int index)
{
    return {};
}

void SOFARAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SOFARAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    convolution.prepare(getTotalNumInputChannels());
    updateIR();
}

void SOFARAudioProcessor::releaseResources()
{
    currentIR.clear();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SOFARAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SOFARAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto distance = parameters.getRawParameterValue("distance")->load();
    auto space    = (int) parameters.getRawParameterValue("space")->load();

    if (space != currentIndex)
        updateIR();

    juce::AudioBuffer<float> wetBuffer;
    wetBuffer.makeCopyOf (buffer);
    convolution.process (wetBuffer);

    float dryGain = 1.0f - distance;
    float wetGain = distance;

    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        auto* dry  = buffer.getWritePointer (ch);
        auto* wet  = wetBuffer.getReadPointer (ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            dry[i] = dry[i] * dryGain + wet[i] * wetGain;
    }
}

//==============================================================================
bool SOFARAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SOFARAudioProcessor::createEditor()
{
    return new SOFARAudioProcessorEditor (*this);
}

//==============================================================================
void SOFARAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SOFARAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
    updateIR();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SOFARAudioProcessor();
}

void SOFARAudioProcessor::updateIR()
{
    int index = (int) parameters.getRawParameterValue("space")->load();
    if (index != currentIndex)
    {
        currentIR = getIR(index);
        convolution.loadImpulse(currentIR);
        convolution.prepare(getTotalNumInputChannels());
        currentIndex = index;
    }
}
