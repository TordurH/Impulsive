/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ImpulsiveAudioProcessor::ImpulsiveAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), avpts(*this, nullptr, "ParameterTree", setLayout())
#endif
{
    formatManager.registerBasicFormats();
    //Temporary. We want to be aable to load custom IRs later
    darkIR.loadImpulseResponse(BinaryData::IR__Audio_01_wav, BinaryData::IR__Audio_01_wavSize, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::no);
    darkIR.loadImpulseResponse(BinaryData::IR__Audio_02_wav, BinaryData::IR__Audio_02_wavSize, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::yes, 0, juce::dsp::Convolution::Normalise::no);

}

ImpulsiveAudioProcessor::~ImpulsiveAudioProcessor()
{
    
}

//==============================================================================
const juce::String ImpulsiveAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ImpulsiveAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ImpulsiveAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ImpulsiveAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ImpulsiveAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ImpulsiveAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ImpulsiveAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ImpulsiveAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ImpulsiveAudioProcessor::getProgramName (int index)
{
    return {};
}

void ImpulsiveAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ImpulsiveAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumInputChannels();

    response.prepare(spec);
    darkIR.prepare(spec);
    brightIR.prepare(spec);

}

void ImpulsiveAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ImpulsiveAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ImpulsiveAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    juce::dsp::AudioBlock<float> block(buffer);
    
    juce::dsp::ProcessContextReplacing<float> context(block);

    if (darkOn) darkIR.process(context);
    else if (brightOn) brightIR.process(context);

    response.process(context);
}

//==============================================================================
bool ImpulsiveAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ImpulsiveAudioProcessor::createEditor()
{
    return new ImpulsiveAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ImpulsiveAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ImpulsiveAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ImpulsiveAudioProcessor();
}

void ImpulsiveAudioProcessor::loadFile(){

    
    chooser = std::make_unique<juce::FileChooser>("Enter an impulse response");

    std::function<void(const juce::FileChooser&)> callback = [&](const juce::FileChooser &c) {
        
        juce::File file = chooser.get()->getResult();

        if (!file.existsAsFile()) chooser.release();
        
        formatReader = formatManager.createReaderFor(file);

        response.loadImpulseResponse(file,juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::yes, 0,juce::dsp::Convolution::Normalise::yes);
        };
    
    chooser.get()->launchAsync(0, callback);
    



}


juce::AudioProcessorValueTreeState::ParameterLayout ImpulsiveAudioProcessor::setLayout() {

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    juce::StringArray irListIndex("0");

    

    layout.add(
        std::make_unique<juce::AudioParameterBool>("Dark", "Dark", false),
        std::make_unique<juce::AudioParameterBool>("Bright", "Bright", false),
        std::make_unique<juce::AudioParameterChoice>("IrList", "IrList", irListIndex, 0)

    );

        return layout;

}