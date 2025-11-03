/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class ImpulsiveAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ImpulsiveAudioProcessor();
    ~ImpulsiveAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool bright, dark;

    juce::AudioProcessorValueTreeState::ParameterLayout setLayout();

    juce::AudioProcessorValueTreeState avpts;

    void loadFile();


    bool darkOn = false, brightOn = false;

private:


    juce::dsp::Convolution response, darkIR, brightIR;
    //juce::File files[5];

    std::unique_ptr<juce::FileChooser> chooser;
    juce::AudioFormatManager formatManager;
    juce::AudioFormatReader* formatReader = nullptr;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ImpulsiveAudioProcessor)
};
