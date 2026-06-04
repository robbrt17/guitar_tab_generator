#pragma once
#include <JuceHeader.h>
#include <cmath>

struct GuitarNote
{
    float timestamp;
    int stringNum; // 1 to 6 (1 = High e, 6 = Low E)
    int fret;
    int bendAmount;
};

class TabTranscriber
{
public:
    static GuitarNote convertHzToTab(float frequency, float timestamp)
    {
        GuitarNote note { timestamp, 6, 0 };

        if (frequency < 40.0f) return note; // Ignore noise below guitar range

        // 1. Convert Hz to standard MIDI Note Number
        int midiNote = std::round(69.0f + 12.0f * std::log2(frequency / 440.0f));

        // 2. Standard Guitar Tuning MIDI roots (E2=40, A2=45, D3=50, G3=55, B3=59, e4=64)
        int stringRoots[6] = { 64, 59, 55, 50, 45, 40 };

        // 3. Find which string makes the most sense (preferring lower frets)
        for (int i = 0; i < 6; ++i)
        {
            int fret = midiNote - stringRoots[i];
            if (fret >= 0 && fret <= 22) // Fits on a standard fretboard
            {
                note.stringNum = i + 1;
                note.fret = fret;
                break; // Found our placement!
            }
        }

        return note;
    }
};