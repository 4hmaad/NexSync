/*
  ==============================================================================

    DeckGUI.cpp
    Created: 13 Mar 2020 6:44:48pm
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(DJAudioPlayer *_player,
                 juce::AudioFormatManager &formatManagerToUse,
                 juce::AudioThumbnailCache &cacheToUse
) : player(_player),
    waveformDisplay(formatManagerToUse, cacheToUse, _player) {

    addAndMakeVisible(playStopButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(volSliderLabel);
    addAndMakeVisible(speedSliderLabel);

    playStopButton.addListener(this);
    playStopButton.setEnabled(false);

    volSlider.addListener(this);
    speedSlider.addListener(this);

    // by default, the loop button is off
    loopButton.addListener(this);
    loopButton.setToggleState(false, juce::dontSendNotification);

    volSlider.setRange(0.0, 1.0);
    speedSlider.setRange(1, 200.0, 1.0);

    volSlider.setValue(0.8);
    volSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 50, 20);

    speedSlider.setValue(100.0);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);


    speedSliderLabel.setText("Speed:", juce::dontSendNotification);
    volSliderLabel.setText("Volume:", juce::dontSendNotification);

    startTimer(500);
}

DeckGUI::~DeckGUI() {
    stopTimer();
}

void DeckGUI::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::darkgrey);
    g.drawRect(getLocalBounds().removeFromTop(50), 2);

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
}

void DeckGUI::resized() {
    juce::Grid grid;
    using Track = juce::Grid::TrackInfo;

    grid.templateRows = {Track(juce::Grid::Fr(1))};
    grid.templateColumns = { Track(juce::Grid::Fr(3)),
                             Track(juce::Grid::Fr(1)),
                             Track(juce::Grid::Fr(3)),
                             Track(juce::Grid::Fr(1)),
                             Track(juce::Grid::Fr(3)),
                             Track(juce::Grid::Fr(1))};
    grid.items = {
            juce::GridItem(playStopButton).withArea(1, 1).withMargin(juce::GridItem::Margin(10)),
            juce::GridItem(volSliderLabel).withArea(1, 2).withMargin(juce::GridItem::Margin(10)),
            juce::GridItem(volSlider).withArea(1, 3).withMargin(juce::GridItem::Margin(10)),
            juce::GridItem(speedSliderLabel).withArea(1, 4).withMargin(juce::GridItem::Margin(10)),
            juce::GridItem(speedSlider).withArea(1, 5).withMargin(juce::GridItem::Margin(10)),
            juce::GridItem(loopButton).withArea(1, 6).withMargin(juce::GridItem::Margin(10)),
    };

    grid.performLayout(getLocalBounds().removeFromTop(50));
    waveformDisplay.setBounds(0, 50, getWidth(), getHeight() - 50);
}

void DeckGUI::buttonClicked(juce::Button *button) {
    if (button == &playStopButton) {
        std::cout << "Play button was clicked " << std::endl;
        if (player->isPlaying()) {
            player->stop();
        } else {
            player->start();
        }

        updatePlayStopButton();
    }
    if (button == &loopButton) {
        std::cout << "Loop button was clicked " << std::endl;
        player->setLooping(loopButton.getToggleState());
        player->start();
    }
}

void DeckGUI::sliderValueChanged(juce::Slider *slider) {
    if (slider == &volSlider) {
        player->setGain(slider->getValue());
    }
    if (slider == &speedSlider) {
        player->setSpeed(slider->getValue() / 100);
    }
}

bool DeckGUI::isInterestedInFileDrag(const juce::StringArray &files) {
    std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
    return true;
}

void DeckGUI::filesDropped(const juce::StringArray &files, int x, int y) {
    std::cout << "DeckGUI::filesDropped" << std::endl;
    if (files.size() == 1) {
        playTrack(juce::URL{juce::File{files[0]}});
    }
}

bool DeckGUI::isInterestedInDragSource(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) {

    // make sure it is a track object
    auto trackObj = dragSourceDetails.description.getDynamicObject();
    if (trackObj == nullptr) {
        return false;
    }

    return true;
}

void DeckGUI::itemDropped(const juce::DragAndDropTarget::SourceDetails &dragSourceDetails) {
    std::cout << "DeckGUI::itemDropped" << std::endl;

    auto trackObj = dragSourceDetails.description.getDynamicObject();
    juce::String trackFileURL = trackObj->getProperty("fileURL");

    playTrack(juce::URL{trackFileURL});
}

void DeckGUI::timerCallback() {
    waveformDisplay.setPositionRelative(
            player->getPositionRelative());

    // reset player when the track is finished
    if (player->isFinished()) {
        player->setPositionRelative(0.0);
        updatePlayStopButton();
    }
}

void DeckGUI::playTrack(juce::URL trackURL) {
    player->loadURL(trackURL);
    waveformDisplay.loadURL(juce::URL{trackURL});

    updatePlayStopButton();
}

void DeckGUI::updatePlayStopButton() {
    // if the player is not loaded, disable the play button and terminate the function
    if (player->isLoaded() == false) {
        playStopButton.setEnabled(false);
        loopButton.setEnabled(false);
        return;
    }

    // otherwise, enable the play button
    playStopButton.setEnabled(true);
    loopButton.setEnabled(true);

    // and set the button text to "Play" or "Stop" depending on the player state
    if (player->isPlaying()) {
        playStopButton.setButtonText("Stop");
    } else {
        playStopButton.setButtonText("Play");
    }
}