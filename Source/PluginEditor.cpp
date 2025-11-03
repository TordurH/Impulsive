/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ImpulsiveAudioProcessorEditor::ImpulsiveAudioProcessorEditor (ImpulsiveAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    addAndMakeVisible(&darkButton);
    addAndMakeVisible(&lightButton);
    addAndMakeVisible(&loadFileButton);


    //darkButton.setClickingTogglesState(true);
    //lightButton.setClickingTogglesState(true);

    darkButton.setButtonText("DARK");
    darkButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::black);
    darkButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::lightgrey);

    lightButton.setButtonText("LIGHT");
    lightButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::lightgrey);
    lightButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::black);

    loadFileButton.setButtonText("No IR loaded");
    loadFileButton.onClick = [&]() { audioProcessor.loadFile(); };

    darkButton.onClick = [&]() { audioProcessor.darkOn = !audioProcessor.darkOn; };
    lightButton.onClick = [&]() { audioProcessor.brightOn = !audioProcessor.brightOn; };
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

ImpulsiveAudioProcessorEditor::~ImpulsiveAudioProcessorEditor()
{
}

//==============================================================================
void ImpulsiveAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

   
}

void ImpulsiveAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getBounds().reduced(5);
    int windowWidth = getWidth();
    int windowHeight = getHeight();

    juce::Rectangle<int> buttonArea = bounds.removeFromTop(windowHeight / 4);
    juce::Rectangle<int> lowerArea = bounds;

    darkButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2));

    lightButton.setBounds(buttonArea);

    loadFileButton.setBounds(bounds.reduced(5));

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
