#include "PluginProcessor.h"
#include "PluginEditor.h"

StaticWaveshaperAudioProcessorEditor::StaticWaveshaperAudioProcessorEditor(
    StaticWaveshaperAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(460, 260);

    titleLabel.setText("Static Memoryless Waveshaper", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(22.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    setupSlider(driveSlider);
    setupSlider(levelSlider);

    driveSlider.setRange(0.0, 36.0, 0.01);
    levelSlider.setRange(-24.0, 12.0, 0.01);

    driveSlider.setTextValueSuffix(" dB");
    levelSlider.setTextValueSuffix(" dB");

    addAndMakeVisible(driveSlider);
    addAndMakeVisible(levelSlider);

    setupLabel(driveLabel, "Drive");
    setupLabel(levelLabel, "Level");
    setupLabel(modeLabel, "Clipping Mode");

    addAndMakeVisible(driveLabel);
    addAndMakeVisible(levelLabel);
    addAndMakeVisible(modeLabel);

    modeBox.addItem("Hard Clip", 1);
    modeBox.addItem("tanh Soft Clip", 2);
    modeBox.addItem("atan Soft Clip", 3);
    modeBox.addItem("Cubic Soft Clip", 4);
    addAndMakeVisible(modeBox);

    driveAttachment = std::make_unique<SliderAttachment>(
        audioProcessor.apvts, "drive", driveSlider);

    levelAttachment = std::make_unique<SliderAttachment>(
        audioProcessor.apvts, "level", levelSlider);

    modeAttachment = std::make_unique<ComboBoxAttachment>(
        audioProcessor.apvts, "mode", modeBox);
}

StaticWaveshaperAudioProcessorEditor::~StaticWaveshaperAudioProcessorEditor()
{
}

void StaticWaveshaperAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(22, 22, 24));

    g.setColour(juce::Colour(45, 45, 50));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(12.0f), 12.0f);

    g.setColour(juce::Colours::white);
}

void StaticWaveshaperAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    titleLabel.setBounds(bounds.removeFromTop(40));

    bounds.removeFromTop(15);

    auto controlArea = bounds.removeFromTop(130);

    auto driveArea = controlArea.removeFromLeft(140);
    auto levelArea = controlArea.removeFromLeft(140);
    auto modeArea = controlArea;

    driveLabel.setBounds(driveArea.removeFromTop(24));
    driveSlider.setBounds(driveArea);

    levelLabel.setBounds(levelArea.removeFromTop(24));
    levelSlider.setBounds(levelArea);

    modeLabel.setBounds(modeArea.removeFromTop(24));
    modeBox.setBounds(modeArea.removeFromTop(32).reduced(10, 0));
}

void StaticWaveshaperAudioProcessorEditor::setupSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 22);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(255, 140, 60));
    slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void StaticWaveshaperAudioProcessorEditor::setupLabel(juce::Label& label,
                                                      const juce::String& text)
{
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::Font(15.0f, juce::Font::bold));
}