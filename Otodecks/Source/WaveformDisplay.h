/*
  ==============================================================================

    WaveformDisplay.h
    Created: 14 Mar 2020 3:50:16pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
*/
class WaveformDisplay : public juce::Component,
                        public juce::ChangeListener {
public:
    WaveformDisplay(juce::AudioFormatManager &formatManagerToUse,
                    juce::AudioThumbnailCache &cacheToUse);

    ~WaveformDisplay();

    void paint(juce::Graphics &) override;

    void resized() override;

    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    void loadURL(juce::URL audioURL);

    /** set the relative position of the playhead*/
    void setPositionRelative(double pos);

    /** allows user to update the position of the playhead by clicking on the waveform */
    void mouseDown(const juce::MouseEvent &event) override;

private:
    juce::AudioThumbnail audioThumb;
    bool fileLoaded;
    double position;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
