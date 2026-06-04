#pragma once
#include <JuceHeader.h>
#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <algorithm>

class OnsetDetector
{
public:
    static std::vector<float> detectOnsets(const juce::AudioBuffer<float>& buffer, double sampleRate)
    {
        std::vector<float> onsetTimes;

        const int order = 10;
        const int windowSize = 1 << order; // 1024
        const int hopSize = 512;
        
        juce::dsp::FFT forwardFFT (order);
        juce::dsp::WindowingFunction<float> window (windowSize, juce::dsp::WindowingFunction<float>::hann);

        auto* readPointer = buffer.getReadPointer(0);
        int totalSamples = buffer.getNumSamples();

        std::vector<float> fftData (windowSize * 2, 0.0f);
        std::vector<float> previousMagnitude (windowSize / 2, 0.0f);
        
        // --- ADAPTIVE THRESHOLD VARIABLES ---
        float movingAverageFlux = 0.0f;
        
        // Tweak this multiplier. 
        // 1.5 means the spike must be 50% louder than the current average ringing noise.
        const float thresholdMultiplier = 2.5f; 
        
        // A tiny baseline to prevent dividing by zero or triggering on pure silence
        const float noiseFloor = 5.0f; 

        const float cooldownSeconds = 0.05f; 
        float lastOnsetTime = -1.0f;

        for (int i = 0; i < totalSamples - windowSize; i += hopSize)
        {
            // 1. Prepare and run FFT
            std::fill (fftData.begin(), fftData.end(), 0.0f);
            for (int j = 0; j < windowSize; ++j) {
                fftData[j] = readPointer[i + j];
            }

            window.multiplyWithWindowingTable (fftData.data(), windowSize);
            forwardFFT.performFrequencyOnlyForwardTransform (fftData.data());

            // 2. Calculate Spectral Flux (Only positive energy changes)
            float currentFlux = 0.0f;
            for (int bin = 0; bin < windowSize / 2; ++bin)
            {
                float magnitude = fftData[bin];
                float difference = magnitude - previousMagnitude[bin];
                
                // Half-wave rectification: we only care if a frequency got LOUDER
                if (difference > 0.0f) {
                    currentFlux += difference;
                }
                
                // Save current magnitude for the next loop
                previousMagnitude[bin] = magnitude;
            }

            // 3. Calculate the Adaptive Threshold
            // This slowly tracks the average flux. 0.8 keeps it smooth but responsive.
            movingAverageFlux = (movingAverageFlux * 0.8f) + (currentFlux * 0.2f);
            
            float adaptiveThreshold = (movingAverageFlux * thresholdMultiplier) + noiseFloor;
            float timeInSecs = static_cast<float>(i) / static_cast<float>(sampleRate);

            // Uncomment to debug the math:
            // juce::Logger::writeToLog("Time: " + juce::String(timeInSecs) + " | Flux: " + juce::String(currentFlux) + " | Threshold: " + juce::String(adaptiveThreshold));

            // 4. Peak Picking
            if (currentFlux > adaptiveThreshold)
            {
                if (timeInSecs - lastOnsetTime > cooldownSeconds) 
                {
                    onsetTimes.push_back(timeInSecs);
                    lastOnsetTime = timeInSecs;
                    
                    // Artificial boost to the average so it doesn't double-trigger on the decay of the same spike
                    movingAverageFlux += currentFlux * 0.5f; 
                }
            }
        }

        return onsetTimes;
    }
};