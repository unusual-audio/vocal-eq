/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalEQAudioProcessor::VocalEQAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter (highPassFilter = new juce::AudioParameterInt (
        "highPassFilter", "High pass filter", 20, 200, 20));
    addParameter (trebleBoost = new juce::AudioParameterFloat (
        "trebleBoost", "Treble boost", 0.0f, 1.0f, 0.0f));
    addParameter (rollOff = new juce::AudioParameterFloat (
        "rollOff", "Roll off", 0.0f, 1.0f, 0.0f));
}

VocalEQAudioProcessor::~VocalEQAudioProcessor()
{
}

//==============================================================================
const juce::String VocalEQAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VocalEQAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VocalEQAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VocalEQAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VocalEQAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VocalEQAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VocalEQAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VocalEQAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VocalEQAudioProcessor::getProgramName (int index)
{
    return {};
}

void VocalEQAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VocalEQAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    lowCut[0].reset();
    lowCut[1].reset();

    highPeak[0].reset();
    highPeak[1].reset();
    
    highShelf[0].reset();
    highShelf[1].reset();
}

void VocalEQAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VocalEQAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void VocalEQAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    lowCut[0].setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), *highPassFilter));
    lowCut[1].setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), *highPassFilter));
    
    highPeak[0].setCoefficients(juce::IIRCoefficients::makePeakFilter(getSampleRate(), 9000, 0.5, 1 + *trebleBoost));
    highPeak[1].setCoefficients(juce::IIRCoefficients::makePeakFilter(getSampleRate(), 9000, 0.5, 1 + *trebleBoost));
    
    highShelf[0].setCoefficients(juce::IIRCoefficients::makeHighShelf(getSampleRate(), 8000 - 7000 * *rollOff, 0.5, 1 - *rollOff * 0.5));
    highShelf[1].setCoefficients(juce::IIRCoefficients::makeHighShelf(getSampleRate(), 8000 - 7000 * *rollOff, 0.5, 1 - *rollOff * 0.5));

    for (auto i = 0; i < buffer.getNumChannels(); ++i)
    {
        if (*highPassFilter > 20)
            lowCut[i].processSamples(buffer.getWritePointer(i), buffer.getNumSamples());
        if (*trebleBoost > 0)
            highPeak[i].processSamples(buffer.getWritePointer(i), buffer.getNumSamples());
        if (*rollOff > 0)
            highShelf[i].processSamples(buffer.getWritePointer(i), buffer.getNumSamples());
    }

}

//==============================================================================
bool VocalEQAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VocalEQAudioProcessor::createEditor()
{
    return new VocalEQAudioProcessorEditor (*this);
}

//==============================================================================
void VocalEQAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("VocalEQParameter"));
    xml->setAttribute ("highPassFilter", (double) *highPassFilter);
    xml->setAttribute ("trebleBoost", (double) *trebleBoost);
    xml->setAttribute ("rollOff", (double) *rollOff);
    copyXmlToBinary (*xml, destData);
}

void VocalEQAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName ("VocalEQParameter"))
        {
            *highPassFilter = (float) xmlState->getDoubleAttribute ("highPassFilter", *highPassFilter);
            *trebleBoost = (float) xmlState->getDoubleAttribute ("trebleBoost", *trebleBoost);
            *rollOff = (float) xmlState->getDoubleAttribute ("rollOff", *rollOff);
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalEQAudioProcessor();
}
