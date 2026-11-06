# ANWAR CH 04 SYNTH — Windows VST3 starter project

A JUCE/C++ polyphonic MIDI instrument project with a dark emerald animated interface, six selectable synthesis patches, master level, attack, release and reverb.

## Important sound-quality note
This is a working **synthesizer prototype**, not a finished sampled-instrument library. The six instrument slots currently use different oscillator tone recipes. Realistic piano, acoustic/electric guitar and violin require licensed, high-quality multisampled WAV/AIFF recordings, velocity layers, round robins and instrument-specific articulation programming. Those recordings are not included.

## Build on Windows 10/11 (64-bit)
1. Install Visual Studio 2022 with **Desktop development with C++**.
2. Install CMake 3.22 or newer.
3. Download JUCE 8 from https://juce.com/get-juce/ and install it.
4. Configure the project, setting `JUCE_DIR` to JUCE's CMake folder, for example:
   `C:\JUCE\ CMake` (use the actual folder path on your PC).
5. From a Developer PowerShell:
   ```
   cmake -S . -B build -DJUCE_DIR="C:/JUCE"
   cmake --build build --config Release
   ```
   The exact `JUCE_DIR` must point to the directory containing `JUCEConfig.cmake`.
6. The VST3 will be produced under the build tree, typically in:
   `build/ANWARCH04SYNTH_artefacts/Release/VST3/ANWAR CH 04 SYNTH.vst3`
7. Copy the `.vst3` bundle to:
   `C:\Program Files\Common Files\VST3`
   Then rescan plugins in your DAW.

## Included controls
- Instrument selector: Acoustic Piano, Acoustic Guitar, Electric Guitar, Violin, Synth, Reel Keys
- Master, Attack, Release, Reverb
- Animated green spectrum-style display
- MIDI note input, 32-voice allocation
- Parameter and selected-instrument state recall

## Next development stage
Add a sample-bank loader and licensed multisampled instrument content; improve guitar pluck/body modelling, violin bow expression, piano velocity layers, stereo imaging, filters, chorus, delay and preset management.
