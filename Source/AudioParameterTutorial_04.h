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

/*******************************************************************************
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             AudioParameterTutorial
 version:          4.0.0
 vendor:           JUCE
 website:          http://juce.com
 description:      Explores audio plugin parameters.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
                   juce_audio_plugin_client, juce_audio_processors,
                   juce_audio_utils, juce_core, juce_data_structures,
                   juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             AudioProcessor
 mainClass:        TutorialProcessor

 useLocalCopy:     1

 END_JUCE_PIP_METADATA

*******************************************************************************/


#pragma once

//==============================================================================
class TutorialProcessor  : public juce::AudioProcessor
{
public:

    //==============================================================================
    TutorialProcessor()
    {
        addParameter (gain        = new juce::AudioParameterFloat ("gain", "Gain", 0.0f, 1.0f, 0.5f));
        addParameter (invertPhase = new juce::AudioParameterBool  ("invertPhase", "Invert Phase", false)); // [3]
    }

    //==============================================================================
    void prepareToPlay (double, int) override
    {
        auto phase = *invertPhase ? -1.0f : 1.0f;
        previousGain = *gain * phase;
    }

    void releaseResources() override {}

    void processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        auto phase = *invertPhase ? -1.0f : 1.0f;  // [6]
        auto currentGain = *gain * phase;          // [7]

        if (currentGain == previousGain)
        {
            buffer.applyGain (currentGain);
        }
        else
        {
            buffer.applyGainRamp (0, buffer.getNumSamples(), previousGain, currentGain);
            previousGain = currentGain;
        }
    }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override          { return new juce::GenericAudioProcessorEditor (*this); }
    bool hasEditor() const override                              { return true; }

    //==============================================================================
    const juce::String getName() const override                  { return "Param Tutorial"; }
    bool acceptsMidi() const override                            { return false; }
    bool producesMidi() const override                           { return false; }
    double getTailLengthSeconds() const override                 { return 0; }

    //==============================================================================
    int getNumPrograms() override                                { return 1; }
    int getCurrentProgram() override                             { return 0; }
    void setCurrentProgram (int) override                        {}
    const juce::String getProgramName (int) override             { return {}; }
    void changeProgramName (int, const juce::String&) override   {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override
    {
        std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("ParamTutorial"));
        xml->setAttribute ("gain", (double) *gain);
        xml->setAttribute ("invertPhase", *invertPhase); // [4]
        copyXmlToBinary (*xml, destData);
    }

    void setStateInformation (const void* data, int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

        if (xmlState.get() != nullptr)
        {
            if (xmlState->hasTagName ("ParamTutorial"))
            {
                *gain = (float) xmlState->getDoubleAttribute ("gain", 1.0);
                *invertPhase = xmlState->getBoolAttribute ("invertPhase", false); // [5]
            }
        }
    }

private:
    //==============================================================================
    juce::AudioParameterFloat* gain;
    juce::AudioParameterBool* invertPhase; // [2]

    float previousGain;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TutorialProcessor)
};
