#include "PluginEditor.h"
#include <cmath>

static const juce::Colour kAmber (0xfff4a63c);
static const juce::Colour kAmberDim (0xffc98a2e);
static const float kStartAngle = juce::MathConstants<float>::pi * 1.25f;
static const float kEndAngle   = juce::MathConstants<float>::pi * 2.75f;

void VocalLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                         juce::Slider&)
{
    const float cx = (float) x + (float) width * 0.5f;
    const float cy = (float) y + (float) height * 0.5f;
    const float r = juce::jmin (width, height) * 0.5f - 14.0f;

    for (int k = 0; k < 5; ++k)
    {
        g.setColour (juce::Colours::black.withAlpha (0.07f));
        const float e = (float) k;
        g.fillEllipse (cx - r - e, cy - r + 3.0f + e, 2.0f * (r + e), 2.0f * (r + e));
    }

    g.setColour (juce::Colours::black.withAlpha (0.7f));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = rotaryStartAngle + ((float) i / 10.0f) * (rotaryEndAngle - rotaryStartAngle);
        const float px = cx + (r + 6.0f) * std::sin (a);
        const float py = cy - (r + 6.0f) * std::cos (a);
        g.fillEllipse (px - 1.2f, py - 1.2f, 2.4f, 2.4f);
    }

    g.setColour (juce::Colour (0xff0d0d0d));
    g.fillEllipse (cx - r, cy - r, 2.0f * r, 2.0f * r);
    g.setColour (juce::Colour (0xff2c2c2c));
    g.drawEllipse (cx - r, cy - r, 2.0f * r, 2.0f * r, 3.0f);

    const float a = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    juce::Line<float> line (cx + 7.0f * std::sin (a), cy - 7.0f * std::cos (a),
                            cx + (r - 5.0f) * std::sin (a), cy - (r - 5.0f) * std::cos (a));
    g.setColour (kAmber);
    g.drawLine (line, 3.2f);
}

VocalAudioProcessorEditor::VocalAudioProcessorEditor (VocalAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setSize (760, 580);

    auto add = [this] (const char* id, const juce::String& name, const juce::String& section, int fmt,
                       bool gain, int cx, int cy, float R,
                       std::vector<std::pair<float, juce::String>> labels)
    {
        specs.push_back ({ id, name, section, fmt, gain, cx, cy, R, std::move (labels) });
    };

    // Acabado
    add (ids::aire, "AIRE", "ACABADO", fmtPlain, false, 505, 82, 36.0f,
         { { 0.0f, "0" }, { 2.5f, "2.5" }, { 5.0f, "5" }, { 7.5f, "7.5" }, { 10.0f, "10" } });
    add (ids::sat, "SATURACION", "ACABADO", fmtPlain, false, 595, 82, 36.0f,
         { { 0.0f, "0" }, { 2.5f, "2.5" }, { 5.0f, "5" }, { 7.5f, "7.5" }, { 10.0f, "10" } });
    add (ids::out, "SALIDA dB", "ACABADO", fmtDbSigned, true, 685, 82, 36.0f,
         { { -12.0f, "-12" }, { 12.0f, "+12" }, { 0.0f, "0" } });

    // Ecualizador
    const char* bands[4] = { "GRAVES", "MEDIOS", "PRESENCIA", "AGUDOS" };
    const int xs[8] = { 79, 165, 251, 337, 423, 509, 595, 681 };
    const std::vector<std::pair<float, juce::String>> freqLabels[4] =
    {
        { { 20.0f, "20" }, { 60.0f, "60" }, { 120.0f, "120" }, { 180.0f, "180" }, { 250.0f, "250" } },
        { { 200.0f, "200" }, { 400.0f, "400" }, { 600.0f, "600" }, { 800.0f, "800" }, { 1000.0f, "1k" } },
        { { 1000.0f, "1k" }, { 2000.0f, "2k" }, { 4000.0f, "4k" }, { 6000.0f, "6k" }, { 8000.0f, "8k" } },
        { { 5000.0f, "5k" }, { 8000.0f, "8k" }, { 12000.0f, "12k" }, { 16000.0f, "16k" }, { 20000.0f, "20k" } }
    };

    for (int b = 0; b < 4; ++b)
    {
        add (ids::eqF[b], "FREQ Hz", bands[b], fmtHz, false, xs[b * 2], 277, 34.0f, freqLabels[b]);
        add (ids::eqG[b], "GAIN dB", bands[b], fmtDbSigned, true, xs[b * 2 + 1], 277, 34.0f,
             { { -12.0f, "-12" }, { 12.0f, "+12" }, { 0.0f, "0" } });
    }

    // Compresor
    add (ids::thr, "THRESHOLD", "COMPRESOR", fmtDb, false, 82, 429, 36.0f,
         { { -60.0f, "-60" }, { -45.0f, "-45" }, { -30.0f, "-30" }, { -15.0f, "-15" }, { 0.0f, "0dB" } });
    add (ids::atk, "ATAQUE ms", "COMPRESOR", fmtMs, false, 174, 429, 36.0f,
         { { 0.1f, "0.1" }, { 1.0f, "1" }, { 10.0f, "10" }, { 50.0f, "50" }, { 100.0f, "100" } });
    add (ids::rel, "RELEASE ms", "COMPRESOR", fmtMs, false, 266, 429, 36.0f,
         { { 10.0f, "10" }, { 100.0f, "100" }, { 300.0f, "300" }, { 600.0f, "600" }, { 1000.0f, "1000" } });
    add (ids::makeup, "GAIN", "COMPRESOR", fmtDbSigned, true, 358, 429, 36.0f,
         { { 0.0f, "0dB" }, { 18.0f, "+18dB" } });

    // De-esser
    add (ids::deF, "FRECUENCIA", "DE-ESSER", fmtHz, false, 492, 429, 36.0f,
         { { 4000.0f, "4k" }, { 5000.0f, "5k" }, { 6000.0f, "6k" }, { 8000.0f, "8k" }, { 10000.0f, "10k" } });
    add (ids::deThr, "UMBRAL", "DE-ESSER", fmtDb, false, 585, 429, 36.0f,
         { { -40.0f, "-40" }, { -30.0f, "-30" }, { -20.0f, "-20" }, { -10.0f, "-10" }, { 0.0f, "0dB" } });
    add (ids::deRed, "REDUCCION", "DE-ESSER", fmtDb, false, 678, 429, 36.0f,
         { { 0.0f, "0" }, { 3.0f, "3" }, { 6.0f, "6" }, { 9.0f, "9" }, { 12.0f, "12dB" } });

    for (size_t i = 0; i < specs.size(); ++i)
    {
        auto s = std::make_unique<juce::Slider>();
        s->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        s->setRotaryParameters (kStartAngle, kEndAngle, true);
        s->setLookAndFeel (&lnf);
        addAndMakeVisible (*s);

        attachments.push_back (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            proc.apvts, specs[i].id, *s));

        if (auto* prm = proc.apvts.getParameter (specs[i].id))
            s->setDoubleClickReturnValue (true, proc.apvts.getParameterRange (specs[i].id).convertFrom0to1 (prm->getDefaultValue()));

        const int idx = (int) i;
        auto* raw = s.get();
        s->onDragStart = [this, idx] { lastKnob = idx; repaint (readoutArea); };
        s->onValueChange = [this, idx, raw]
        {
            if (raw->isMouseButtonDown() || raw->isMouseOver())
                lastKnob = idx;
            if (lastKnob == idx)
                repaint (readoutArea);
        };

        sliders.push_back (std::move (s));
    }

    for (auto* b : { &prevButton, &nextButton })
    {
        b->setColour (juce::TextButton::buttonColourId, juce::Colour (0xff0d0d0d));
        b->setColour (juce::TextButton::textColourOffId, kAmber);
        addAndMakeVisible (*b);
    }
    prevButton.onClick = [this] { stepPreset (-1); };
    nextButton.onClick = [this] { stepPreset (1); };

    shownPreset = proc.getCurrentPresetIndex();
    startTimerHz (30);
}

VocalAudioProcessorEditor::~VocalAudioProcessorEditor()
{
    stopTimer();
    for (auto& s : sliders)
        s->setLookAndFeel (nullptr);
}

void VocalAudioProcessorEditor::stepPreset (int delta)
{
    const int n = (int) VocalAudioProcessor::getPresets().size();
    const int cur = proc.getCurrentPresetIndex();
    proc.applyPreset (((cur < 0 ? 0 : cur) + delta + n) % n);
}

void VocalAudioProcessorEditor::timerCallback()
{
    ringRot += 0.012f;
    if (ringRot > juce::MathConstants<float>::twoPi)
        ringRot -= juce::MathConstants<float>::twoPi;
    pulsePhase += 0.07f;
    if (pulsePhase > juce::MathConstants<float>::twoPi)
        pulsePhase -= juce::MathConstants<float>::twoPi;

    grShown += (proc.getGainReduction() - grShown) * 0.3f;

    const int idx = proc.getCurrentPresetIndex();
    if (idx != shownPreset)
    {
        shownPreset = idx;
        repaint();
    }

    repaint (sealArea);
    repaint (meterArea);
}

void VocalAudioProcessorEditor::resized()
{
    for (size_t i = 0; i < sliders.size(); ++i)
        sliders[i]->setBounds (specs[i].cx - 38, specs[i].cy - 38, 76, 76);

    prevButton.setBounds (304, 52, 22, 22);
    nextButton.setBounds (412, 52, 22, 22);
}

juce::String VocalAudioProcessorEditor::formatValue (const KnobSpec& k, double v) const
{
    switch (k.fmt)
    {
        case fmtHz:
            if (v >= 1000.0) return juce::String (v / 1000.0, 1) + " kHz";
            return juce::String ((int) std::round (v)) + " Hz";
        case fmtDb:
            return juce::String (v, 1) + " dB";
        case fmtDbSigned:
            return juce::String (v >= 0.0 ? "+" : "") + juce::String (v, 1) + " dB";
        case fmtMs:
            if (v >= 1000.0) return juce::String (v / 1000.0, 2) + " s";
            if (v < 10.0) return juce::String (v, 1) + " ms";
            return juce::String ((int) std::round (v)) + " ms";
        default:
            return juce::String (v, 1);
    }
}

void VocalAudioProcessorEditor::drawKnobScale (juce::Graphics& g, const KnobSpec& k)
{
    const float cx = (float) k.cx, cy = (float) k.cy;
    g.setColour (juce::Colours::black);
    g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));

    auto text = [&g] (const juce::String& s, float x, float y)
    {
        g.drawText (s, juce::Rectangle<float> (x - 20.0f, y - 6.0f, 40.0f, 12.0f), juce::Justification::centred, false);
    };

    if (k.gainType)
    {
        if (k.labels.size() > 0) text (k.labels[0].second, cx - 22.0f, cy + 36.0f);
        if (k.labels.size() > 1) text (k.labels[1].second, cx + 22.0f, cy + 36.0f);
        if (k.labels.size() > 2) text (k.labels[2].second, cx, cy - 34.0f);
    }
    else
    {
        const auto range = proc.apvts.getParameterRange (k.id);
        for (auto& l : k.labels)
        {
            const float norm = range.convertTo0to1 (l.first);
            const float a = kStartAngle + norm * (kEndAngle - kStartAngle);
            text (l.second, cx + k.scaleRadius * std::sin (a), cy - k.scaleRadius * std::cos (a));
        }
    }

    g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)).withExtraKerningFactor (0.1f));
    g.drawText (k.name, juce::Rectangle<float> (cx - 50.0f, cy + 44.0f, 100.0f, 12.0f), juce::Justification::centred, false);
}

void VocalAudioProcessorEditor::drawSeal (juce::Graphics& g, juce::Point<float> c, float r)
{
    g.setColour (juce::Colours::black.withAlpha (0.16f));
    for (int k = 0; k < 4; ++k)
    {
        const float e = (float) k;
        g.fillEllipse (c.x - r + 2.0f - e, c.y - r + 4.0f - e, 2.0f * r + 2.0f * e, 2.0f * r + 2.0f * e);
    }

    g.setColour (juce::Colour (0xff0d0d0d));
    g.fillEllipse (c.x - r, c.y - r, 2.0f * r, 2.0f * r);

    const float pulse = 0.7f + 0.3f * std::sin (pulsePhase);
    g.setColour (kAmber.withAlpha (pulse));
    const float r2 = r * 0.92f;
    g.drawEllipse (c.x - r2, c.y - r2, 2.0f * r2, 2.0f * r2, 1.2f);

    juce::String ring;
    ring << "KEYWYLL " << juce::String::charToString ((juce::juce_wchar) 0xB7)
         << " AUDIO " << juce::String::charToString ((juce::juce_wchar) 0xB7) << " ";

    const float rt = r * 0.76f;
    g.setFont (juce::Font (juce::FontOptions (6.5f, juce::Font::bold)));
    g.setColour (kAmber);
    const int n = ring.length();
    for (int i = 0; i < n; ++i)
    {
        const float a = ringRot + juce::MathConstants<float>::twoPi * (float) i / (float) n;
        g.saveState();
        g.addTransform (juce::AffineTransform::rotation (a, c.x, c.y));
        g.drawSingleLineText (juce::String::charToString (ring[i]), (int) std::round (c.x),
                              (int) std::round (c.y - rt), juce::Justification::horizontallyCentred);
        g.restoreState();
    }

    g.setFont (juce::Font (juce::FontOptions ("Impact", r * 0.72f, juce::Font::plain)));
    g.drawText ("44", juce::Rectangle<float> (c.x - r * 0.6f, c.y - r * 0.5f, r * 1.2f, r * 0.7f),
                juce::Justification::centred, false);

    g.setFont (juce::Font (juce::FontOptions (7.5f, juce::Font::bold)).withExtraKerningFactor (0.25f));
    g.drawText ("MUSIK", juce::Rectangle<float> (c.x - 20.0f, c.y + r * 0.24f, 40.0f, 9.0f),
                juce::Justification::centred, false);

    g.setFont (juce::Font (juce::FontOptions (5.5f, juce::Font::bold)).withExtraKerningFactor (0.25f));
    g.drawText ("GROUP", juce::Rectangle<float> (c.x - 16.0f, c.y + r * 0.24f + 9.0f, 32.0f, 7.0f),
                juce::Justification::centred, false);
}

void VocalAudioProcessorEditor::drawReadout (juce::Graphics& g)
{
    const auto r = readoutArea.toFloat();
    g.setColour (juce::Colour (0xf0141414));
    g.fillRoundedRectangle (r, 10.0f);
    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle (r, 10.0f, 2.0f);

    g.setColour (kAmberDim);
    g.setFont (juce::Font (juce::FontOptions (11.0f)).withExtraKerningFactor (0.15f));

    if (lastKnob < 0 || lastKnob >= (int) specs.size())
    {
        g.drawText ("LECTURA", r.reduced (16.0f, 8.0f).withHeight (14.0f), juce::Justification::centredLeft, false);
        g.setFont (juce::Font (juce::FontOptions (13.0f)));
        g.drawText ("Mueve una perilla para ver su valor", r, juce::Justification::centred, false);
        return;
    }

    const auto& k = specs[(size_t) lastKnob];
    const double v = sliders[(size_t) lastKnob]->getValue();

    g.drawText ("LECTURA  -  " + k.section + "  -  " + k.name,
                juce::Rectangle<float> (r.getX() + 16.0f, r.getY() + 8.0f, r.getWidth() - 32.0f, 14.0f),
                juce::Justification::centredLeft, false);

    g.setColour (kAmber);
    g.setFont (juce::Font (juce::FontOptions ("Impact", 30.0f, juce::Font::plain)));
    g.drawText (formatValue (k, v),
                juce::Rectangle<float> (r.getX() + 16.0f, r.getY() + 24.0f, 210.0f, 36.0f),
                juce::Justification::centredLeft, false);

    juce::String caption;
    if (k.gainType && k.fmt == fmtDbSigned)
        caption = (v > 0.05) ? "sube el volumen" : (v < -0.05 ? "baja el volumen" : "sin cambio");
    else if (k.fmt == fmtHz)
        caption = "frecuencia seleccionada";
    else
        caption = k.name;

    g.setColour (kAmberDim);
    g.setFont (juce::Font (juce::FontOptions (12.0f)));
    g.drawText (caption,
                juce::Rectangle<float> (r.getX() + 226.0f, r.getY() + 30.0f, r.getWidth() - 242.0f, 28.0f),
                juce::Justification::centredRight, false);
}

void VocalAudioProcessorEditor::paint (juce::Graphics& g)
{
    const auto bounds = juce::Rectangle<float> (4.0f, 4.0f, 752.0f, 572.0f);
    const int w = getWidth(), h = getHeight();

    juce::ColourGradient grad (juce::Colour (0xfff4a63c), 0.0f, 0.0f,
                               juce::Colour (0xffd9821a), 0.0f, (float) h, false);
    g.setGradientFill (grad);
    g.fillRoundedRectangle (bounds, 16.0f);

    juce::Path clip;
    clip.addRoundedRectangle (bounds, 16.0f);
    g.saveState();
    g.reduceClipRegion (clip);
    for (int x = 4; x < w; x += 7)
    {
        g.setColour (juce::Colours::black.withAlpha (0.09f));
        g.fillRect (x, 4, 2, h - 8);
        g.setColour (juce::Colours::white.withAlpha (0.07f));
        g.fillRect (x + 4, 4, 1, h - 8);
    }
    g.restoreState();

    g.setColour (juce::Colour (0xff111111));
    g.drawRoundedRectangle (bounds, 16.0f, 4.0f);

    // Grafiti
    g.setColour (juce::Colours::black.withAlpha (0.85f));
    {
        const juce::PathStrokeType stroke6 (6.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);
        const juce::PathStrokeType stroke5 (5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        juce::Path p1;
        p1.startNewSubPath (300, 112);
        p1.cubicTo (320, 102, 335, 122, 355, 110);
        p1.cubicTo (375, 98, 395, 100, 438, 116);
        g.strokePath (p1, stroke6);

        juce::Path p2;
        p2.startNewSubPath (24, 28);
        p2.lineTo (44, 14);
        p2.lineTo (40, 38);
        p2.lineTo (58, 26);
        g.strokePath (p2, stroke5);

        g.drawEllipse (432.0f, 56.0f, 16.0f, 16.0f, 4.0f);

        juce::Path p3;
        p3.startNewSubPath (36, 536);
        p3.cubicTo (70, 516, 100, 556, 138, 532);
        g.strokePath (p3, stroke6);

        juce::Path p4;
        p4.startNewSubPath (584, 524);
        p4.lineTo (606, 504);
        p4.lineTo (600, 540);
        p4.lineTo (630, 520);
        g.strokePath (p4, stroke5);
    }

    // Titulo
    g.setFont (juce::Font (juce::FontOptions ("Impact", 60.0f, juce::Font::plain)));
    for (int k = 1; k <= 10; ++k)
    {
        g.setColour (juce::Colours::black.withAlpha (0.05f));
        g.drawFittedText ("44VOCAL", juce::Rectangle<int> (36 + 2 + k, 28 + 4 + k, 250, 70),
                          juce::Justification::centredLeft, 1, 0.6f);
    }
    g.setColour (juce::Colours::black);
    g.drawFittedText ("44VOCAL", juce::Rectangle<int> (36, 28, 250, 70), juce::Justification::centredLeft, 1, 0.6f);

    g.setFont (juce::Font (juce::FontOptions (12.0f)));
    g.drawText ("Propiedad de Keywyll", 86, 96, 200, 16, juce::Justification::centredRight);

    // Pantalla de presets
    g.setColour (juce::Colour (0xf0141414));
    g.fillRoundedRectangle (300.0f, 26.0f, 138.0f, 66.0f, 10.0f);
    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle (300.0f, 26.0f, 138.0f, 66.0f, 10.0f, 2.0f);

    const auto& presets = VocalAudioProcessor::getPresets();
    const int idx = juce::jlimit (0, (int) presets.size() - 1, shownPreset < 0 ? 0 : shownPreset);

    g.setColour (kAmberDim);
    g.setFont (juce::Font (juce::FontOptions (11.0f)).withExtraKerningFactor (0.15f));
    g.drawText ("PRESET", 300, 30, 138, 14, juce::Justification::centred);
    g.drawText (juce::String (idx + 1) + " / " + juce::String ((int) presets.size()), 300, 72, 138, 14, juce::Justification::centred);
    g.setColour (kAmber);
    g.setFont (juce::Font (juce::FontOptions ("Impact", 19.0f, juce::Font::plain)));
    g.drawText (presets[(size_t) idx].name, 326, 46, 86, 26, juce::Justification::centred);

    // Paneles
    auto panel = [&g] (float x, float y, float pw, float ph, const juce::String& title, float tx, float ty)
    {
        g.setColour (juce::Colours::black.withAlpha (0.14f));
        g.fillRoundedRectangle (x, y, pw, ph, 12.0f);
        g.setColour (juce::Colour (0xff111111));
        g.drawRoundedRectangle (x, y, pw, ph, 12.0f, 2.0f);
        g.setColour (juce::Colours::black);
        g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold)).withExtraKerningFactor (0.25f));
        g.drawText (title, juce::Rectangle<float> (tx, ty - 12.0f, 200.0f, 16.0f), juce::Justification::centredLeft, false);
    };
    panel (450.0f, 20.0f, 290.0f, 124.0f, "ACABADO", 466.0f, 38.0f);
    panel (20.0f, 152.0f, 720.0f, 187.0f, "ECUALIZADOR", 36.0f, 172.0f);
    panel (20.0f, 349.0f, 400.0f, 144.0f, "COMPRESOR", 36.0f, 371.0f);
    panel (430.0f, 349.0f, 310.0f, 144.0f, "DE-ESSER", 446.0f, 371.0f);

    // Pantalla del ecualizador
    g.setColour (juce::Colour (0xf0141414));
    g.fillRoundedRectangle (36.0f, 178.0f, 688.0f, 30.0f, 6.0f);
    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle (36.0f, 178.0f, 688.0f, 30.0f, 6.0f, 2.0f);

    const float nodeX[4] = { 122.0f, 294.0f, 466.0f, 638.0f };
    float nodeY[4];
    g.setColour (juce::Colour (0xff3a3a3a));
    for (int b = 0; b < 4; ++b)
    {
        g.drawVerticalLine ((int) nodeX[b], 180.0f, 206.0f);
        const float gdb = proc.apvts.getRawParameterValue (ids::eqG[b])->load();
        nodeY[b] = 193.0f - gdb / 12.0f * 11.0f;
    }
    g.drawHorizontalLine (193, 40.0f, 720.0f);

    juce::Path curve;
    curve.startNewSubPath (40.0f, 193.0f);
    curve.lineTo (nodeX[0] - 40.0f, 193.0f);
    float px = nodeX[0] - 40.0f, py = 193.0f;
    for (int b = 0; b < 4; ++b)
    {
        const float mx = (px + nodeX[b]) * 0.5f;
        curve.cubicTo (mx, py, mx, nodeY[b], nodeX[b], nodeY[b]);
        px = nodeX[b]; py = nodeY[b];
    }
    curve.cubicTo ((px + 720.0f) * 0.5f, py, (px + 720.0f) * 0.5f, 193.0f, 720.0f, 193.0f);
    g.setColour (kAmber);
    g.strokePath (curve, juce::PathStrokeType (2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    for (int b = 0; b < 4; ++b)
        g.fillEllipse (nodeX[b] - 3.5f, nodeY[b] - 3.5f, 7.0f, 7.0f);

    g.setColour (juce::Colours::black);
    g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::bold)).withExtraKerningFactor (0.15f));
    const char* bandNames[4] = { "GRAVES", "MEDIOS", "PRESENCIA", "AGUDOS" };
    for (int b = 0; b < 4; ++b)
        g.drawText (bandNames[b], juce::Rectangle<float> (nodeX[b] - 60.0f, 215.0f, 120.0f, 14.0f), juce::Justification::centred, false);

    // Medidor de reduccion del compresor
    g.setColour (juce::Colours::black);
    g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)).withExtraKerningFactor (0.15f));
    g.drawText ("REDUCCION", juce::Rectangle<float> (262.0f, 359.0f, 80.0f, 14.0f), juce::Justification::centredRight, false);
    g.setColour (juce::Colour (0xff141414));
    g.fillRoundedRectangle (344.0f, 361.0f, 60.0f, 10.0f, 3.0f);
    g.setColour (juce::Colours::black);
    g.drawRoundedRectangle (344.0f, 361.0f, 60.0f, 10.0f, 3.0f, 1.5f);
    g.setColour (kAmber);
    g.fillRoundedRectangle (346.0f, 363.0f, 56.0f * juce::jlimit (0.0f, 1.0f, grShown / 18.0f), 6.0f, 2.0f);

    // Numeros de cada perilla
    for (auto& k : specs)
        drawKnobScale (g, k);

    // Pantalla de lectura y sello
    drawReadout (g);
    drawSeal (g, { 696.0f, 535.0f }, 38.0f);
}
