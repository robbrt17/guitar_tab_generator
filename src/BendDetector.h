#pragma once
#include <JuceHeader.h>
#include "PitchDetector.h"
#include <cmath>
#include <algorithm>

class BendDetector
{
public:
    // Returns how many semitones the note was bent (0 = no bend, 1 = half step, 2 = full step)
    static int detectBend(const juce::AudioBuffer<float>& buffer, double sampleRate, float startTime, float endTime, float baseFrequency)
    {
        // If the note is super short (less than 100ms), there's no time for a bend
        if (endTime - startTime < 0.1f) return 0;

        float maxFrequency = baseFrequency;
        
        // We will take a new pitch snapshot every 40 milliseconds
        const float timeStep = 0.04f; 
        
        // Stop scanning slightly before the next note starts so we don't accidentally read the next pick attack
        float scanEndTime = endTime - 0.05f;

        // Loop through the duration of the note
        for (float t = startTime + 0.05f; t < scanEndTime; t += timeStep)
        {
            float currentPitch = PitchDetector::detectPitch(buffer, sampleRate, t);
            
            // Only accept valid pitches to avoid noise throwing off the max
            if (currentPitch > 40.0f && currentPitch < 1500.0f) 
            {
                if (currentPitch > maxFrequency)
                {
                    maxFrequency = currentPitch;
                }
            }
        }

        // Calculate the ratio between the highest pitch found and the starting pitch
        float pitchRatio = maxFrequency / baseFrequency;

        // Convert the mathematical ratio into musical Semitones
        // Formula: 12 * log2(f2 / f1)
        float semitonesBent = 12.0f * std::log2(pitchRatio);

        // Round to the nearest whole fret/semitone
        int roundedBends = std::round(semitonesBent);

        // We only care about intentional bends (ignore tiny vibrato wobbles)
        if (roundedBends >= 1 && roundedBends <= 4) 
        {
            return roundedBends;
        }

        return 0; // No significant bend detected
    }
};