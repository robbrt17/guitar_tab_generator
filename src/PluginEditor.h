#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class StaticWaveshaperAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit StaticWaveshaperAudioProcessorEditor(StaticWaveshaperAudioProcessor&);
    ~StaticWaveshaperAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    StaticWaveshaperAudioProcessor& audioProcessor;

    juce::Slider driveSlider;
    juce::Slider levelSlider;
    juce::ComboBox modeBox;

    juce::Label driveLabel;
    juce::Label levelLabel;
    juce::Label modeLabel;
    juce::Label titleLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> levelAttachment;
    std::unique_ptr<ComboBoxAttachment> modeAttachment;

    void setupSlider(juce::Slider& slider);
    void setupLabel(juce::Label& label, const juce::String& text);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StaticWaveshaperAudioProcessorEditor)
};