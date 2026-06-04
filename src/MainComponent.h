#pragma once
#include <JuceHeader.h>

class MainComponent  : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // The UI Button
    juce::TextButton loadButton { "Load .WAV File" };
    
    // JUCE class that knows how to parse audio file headers
    juce::AudioFormatManager formatManager;
    
    // The file browser window
    std::unique_ptr<juce::FileChooser> chooser;
    
    // The array that will hold our raw audio samples
    juce::AudioBuffer<float> audioBuffer;

    // Our custom function to handle the button click
    void loadButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};