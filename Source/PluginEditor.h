#pragma once
#include "PluginProcessor.h"

class VocalLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& slider) override;
};

class VocalAudioProcessorEditor : public juce::AudioProcessorEditor,
                                  private juce::Timer
{
public:
    explicit VocalAudioProcessorEditor (VocalAudioProcessor&);
    ~VocalAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    enum Fmt { fmtHz, fmtDb, fmtDbSigned, fmtMs, fmtPlain };

    struct KnobSpec
    {
        const char* id;
        juce::String name;
        juce::String section;
        int fmt;
        bool gainType;
        int cx, cy;
        float scaleRadius;
        std::vector<std::pair<float, juce::String>> labels;
    };

    void timerCallback() override;
    void stepPreset (int delta);
    juce::String formatValue (const KnobSpec&, double v) const;
    void drawKnobScale (juce::Graphics&, const KnobSpec&);
    void drawSeal (juce::Graphics&, juce::Point<float> centre, float r);
    void drawReadout (juce::Graphics&);

    VocalAudioProcessor& proc;
    VocalLookAndFeel lnf;

    std::vector<KnobSpec> specs;
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;

    juce::TextButton prevButton { "<" }, nextButton { ">" };

    const juce::Rectangle<int> readoutArea { 170, 501, 390, 64 };
    const juce::Rectangle<int> sealArea { 648, 490, 98, 86 };
    const juce::Rectangle<int> meterArea { 340, 357, 70, 18 };

    int shownPreset = -1;
    int lastKnob = -1;
    float ringRot = 0.0f;
    float pulsePhase = 0.0f;
    float grShown = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalAudioProcessorEditor)
};
