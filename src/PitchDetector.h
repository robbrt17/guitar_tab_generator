#pragma once
#include <JuceHeader.h>
#include <vector>

class PitchDetector
{
public:
    static float detectPitch(const juce::AudioBuffer<float>& buffer, double sampleRate, float onsetTimeInSeconds)
    {
        // FIX 1: Wait 50 milliseconds for the chaotic pick attack noise to die down
        float offsetInSeconds = 0.05f; 
        int startSample = static_cast<int>((onsetTimeInSeconds + offsetInSeconds) * sampleRate);
        
        const int windowSize = 2048; 

        if (startSample + windowSize > buffer.getNumSamples())
            return 0.0f;

        auto* readPointer = buffer.getReadPointer(0);
        
        int minLag = static_cast<int>(sampleRate / 1200.0); 
        int maxLag = static_cast<int>(sampleRate / 75.0);   

        std::vector<float> correlations (maxLag, 0.0f);
        float globalMax = -1.0f;

        // Step 1: Calculate all correlations and find the absolute maximum
        for (int lag = minLag; lag < maxLag; ++lag)
        {
            float sum = 0.0f;
            for (int i = 0; i < windowSize / 2; ++i)
            {
                sum += readPointer[startSample + i] * readPointer[startSample + i + lag];
            }
            correlations[lag] = sum;
            
            if (sum > globalMax) {
                globalMax = sum;
            }
        }

        // FIX 2: The "First Peak" Rule to prevent octave-down errors
        // We don't need the absolute biggest peak; we want the FIRST peak that is 90% as big.
        float threshold = globalMax * 0.90f; 
        int bestLag = -1;

        for (int lag = minLag; lag < maxLag - 1; ++lag)
        {
            if (correlations[lag] > threshold)
            {
                // Check if it is a local peak (higher than the lag before and after it)
                if (correlations[lag] > correlations[lag - 1] && correlations[lag] > correlations[lag + 1])
                {
                    bestLag = lag;
                    break; // WE FOUND IT! Stop searching so we don't accidentally hit the 2nd octave.
                }
            }
        }

        if (bestLag > 0)
        {
            return static_cast<float>(sampleRate) / static_cast<float>(bestLag);
        }

        return 0.0f;
    }
};