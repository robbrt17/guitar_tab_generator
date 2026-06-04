#include "MainComponent.h"
#include "OnsetDetector.h"
#include "PitchDetector.h"
#include "TabTranscriber.h"
#include "BendDetector.h"

MainComponent::MainComponent()
{
    // 1. Tell JUCE we want to be able to read standard formats (like .wav and .aiff)
    formatManager.registerBasicFormats();

    // 2. Setup the button UI
    addAndMakeVisible (loadButton);
    loadButton.onClick = [this] { loadButtonClicked(); };

    // 3. Set the window size
    setSize (400, 300);
}

MainComponent::~MainComponent() {}

void MainComponent::loadButtonClicked()
{
    // 1. Initialize the file chooser to look only for .wav files
    chooser = std::make_unique<juce::FileChooser> ("Select a Wave file to load...",
                                                   juce::File{},
                                                   "*.wav");
    
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    // 2. Launch the file browser asynchronously 
    // (Modern OS's require this so the app doesn't freeze while waiting for the user)
    chooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
    {
        auto file = fc.getResult();

        // If the user actually selected a file (and didn't hit cancel)
        if (file != juce::File{})
        {
            // 3. Try to create an audio reader for this specific file
            if (auto* reader = formatManager.createReaderFor (file))
            {
                // 4. Resize our RAM buffer to match the file's exact length and channels
                audioBuffer.setSize ((int) reader->numChannels, (int) reader->lengthInSamples);
                
                // 5. Copy the audio data from the hard drive into our buffer!
                reader->read (&audioBuffer, 0, (int) reader->lengthInSamples, 0, true, true);
                
                // Print a success message to the IDE console
                juce::Logger::writeToLog("Success! Loaded " + juce::String(reader->lengthInSamples) + " samples into memory.");

                // Get the sample rate of the file (usually 44100.0)
                double sampleRate = reader->sampleRate;

                // RUN THE ONSET DETECTOR!
                // std::vector<float> detectedOnsets = OnsetDetector::detectOnsets(audioBuffer, sampleRate);

                std::vector<float> onsets = OnsetDetector::detectOnsets(audioBuffer, sampleRate);
                std::vector<GuitarNote> allNotes;

                for (int i = 0; i < onsets.size(); ++i)
                {
                    float time = onsets[i];
                    
                    // Find when this note ends (either the next onset, or an arbitrary 1.5 seconds later if it's the last note)
                    float endTime = (i + 1 < onsets.size()) ? onsets[i + 1] : time + 1.5f;

                    // 1. Get the starting pitch
                    float startHz = PitchDetector::detectPitch(audioBuffer, sampleRate, time);
                    
                    if (startHz > 40.0f) 
                    {
                        // 2. Convert to Tab
                        GuitarNote tabNote = TabTranscriber::convertHzToTab(startHz, time);
                        
                        // 3. Scan the duration of the note for a bend!
                        tabNote.bendAmount = BendDetector::detectBend(audioBuffer, sampleRate, time, endTime, startHz);
                        
                        allNotes.push_back(tabNote);
                        
                        juce::String bendLog = (tabNote.bendAmount > 0) ? " (Bends up " + juce::String(tabNote.bendAmount) + " frets)" : "";
                        juce::Logger::writeToLog("Time: " + juce::String(time, 2) + "s | Fret: " + juce::String(tabNote.fret) + bendLog);
                    }
                }
                                
                delete reader; // Clean up the reader to prevent memory leaks
            }
        }
    });
}

void MainComponent::paint (juce::Graphics& g)
{
    // Just a basic dark grey background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    // Place the load button directly in the center of the window
    loadButton.setBounds (getLocalBounds().withSizeKeepingCentre (150, 40));
}