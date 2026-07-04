#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Waveshapers.h"

StaticWaveshaperAudioProcessor::StaticWaveshaperAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
    #if ! JucePlugin_IsMidiEffect
        #if ! JucePlugin_IsSynth
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
        #endif
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    #endif
    ),
#endif
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

StaticWaveshaperAudioProcessor::~StaticWaveshaperAudioProcessor()
{
}

const juce::String StaticWaveshaperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StaticWaveshaperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StaticWaveshaperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StaticWaveshaperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StaticWaveshaperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StaticWaveshaperAudioProcessor::getNumPrograms()
{
    return 1;
}

int StaticWaveshaperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StaticWaveshaperAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String StaticWaveshaperAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void StaticWaveshaperAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void StaticWaveshaperAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void StaticWaveshaperAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StaticWaveshaperAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
   #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
   #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
   #endif
}
#endif

void StaticWaveshaperAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                  juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;

    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const float driveDb = apvts.getRawParameterValue("drive")->load();
    const float levelDb = apvts.getRawParameterValue("level")->load();
    const int clippingMode = static_cast<int>(apvts.getRawParameterValue("mode")->load());

    const float driveGain = juce::Decibels::decibelsToGain(driveDb);
    const float outputGain = juce::Decibels::decibelsToGain(levelDb);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* samples = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float x = samples[sample];

            x *= driveGain;

            float y = Waveshapers::processSample(x, clippingMode);

            samples[sample] = y * outputGain;
        }
    }
}

bool StaticWaveshaperAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* StaticWaveshaperAudioProcessor::createEditor()
{
    return new StaticWaveshaperAudioProcessorEditor(*this);
}

void StaticWaveshaperAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    copyXmlToBinary(*xml, destData);
}

void StaticWaveshaperAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout
StaticWaveshaperAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("drive", 1),
        "Drive",
        juce::NormalisableRange<float>(0.0f, 36.0f, 0.01f),
        12.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("level", 1),
        "Level",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel("dB")));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID("mode", 1),
        "Clipping Mode",
        juce::StringArray
        {
            "Hard Clip",
            "tanh Soft Clip",
            "atan Soft Clip",
            "Cubic Soft Clip"
        },
        1));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StaticWaveshaperAudioProcessor();
}