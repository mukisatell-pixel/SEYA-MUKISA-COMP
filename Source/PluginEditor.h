#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

// ============ KNOBS LOCKED: LIGHT BLUE OUTER + RED ACTION ============
class GreenDarkRedActionLook : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h,
                          float pos, float start, float end, juce::Slider&) override
    {
        auto bounds = juce::Rectangle<float>((float)x,(float)y,(float)w,(float)h).reduced(2.0f);
        auto c = bounds.getCentre();
        float r = bounds.getWidth() * 0.5f;
        float ang = start + pos * (end - start);
        g.setColour(juce::Colour(0xff0A120C));
        g.fillEllipse(c.x - r + 1, c.y - r + 3, r*2, r*2);
        g.setColour(juce::Colour(0xff3AA0FF)); // LIGHT BLUE - locked
        g.drawEllipse(c.x - r, c.y - r, r*2, r*2, 4.5f);
        juce::Path ringPath;
        ringPath.addCentredArc(c.x, c.y, r-3.0f, r-3.0f, 0.0f, start, ang, true);
        g.setColour(juce::Colour(0xffCC1225).withAlpha(0.92f)); // RED ACTION LOCKED
        g.strokePath(ringPath, juce::PathStrokeType(5.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.setColour(juce::Colour(0xffFF2040).withAlpha(0.35f + pos*0.35f));
        g.strokePath(ringPath, juce::PathStrokeType(9.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        auto inner = bounds.reduced(12.0f);
        g.setColour(juce::Colour(0xff060606));
        g.fillEllipse(inner);
        g.setColour(juce::Colour(0xff1A1A1A));
        g.drawEllipse(inner.getX(), inner.getY(), inner.getWidth(), inner.getHeight(), 0.8f);
        bool isBig = w > 125;
        juce::Colour tickCol = isBig? juce::Colour(0xff00FF55) : juce::Colour(0xffFF8A20);
        float len = isBig? r*0.34f : r*0.24f;
        juce::Point<float> p1(c.x + r*0.10f*std::cos(ang-juce::MathConstants<float>::halfPi), c.y + r*0.10f*std::sin(ang-juce::MathConstants<float>::halfPi));
        juce::Point<float> p2(c.x + len*std::cos(ang-juce::MathConstants<float>::halfPi), c.y + len*std::sin(ang-juce::MathConstants<float>::halfPi));
        g.setColour(tickCol);
        g.drawLine(p1.x, p1.y, p2.x, p2.y, isBig? 1.9f : 1.3f);
    }
};

class OrangeLedMeter : public juce::Component
{
public:
    void setLevel(float l){ level = level*0.55f + l*0.45f; repaint(); }
    void paint(juce::Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff0A0A0A)); g.fillRoundedRectangle(b, 4.0f);
        float segs[] = {8,6,4,2,0,-2,-4,-8,-12,-16,-20,-28,-40,-60};
        int n=14; float h=(b.getHeight()-8)/n;
        float lvlDb = juce::Decibels::gainToDecibels(level+0.0001f);
        for(int i=0;i<n;i++)
        {
            float y=b.getY()+4+i*h; bool on = lvlDb > segs[i];
            juce::Colour col = (i==1)? juce::Colour(0xffFF5533) : juce::Colour(0xffFFA530);
            if(i==0) col = juce::Colour(0xff4A2A1A);
            if(on){g.setColour(col.withAlpha(0.4f)); g.fillRoundedRectangle(b.getX()+2,y+1,b.getWidth()-4,h-1.5f,1.5f);}
            g.setColour(on? col : juce::Colour(0xff1A1A1A).withAlpha(0.6f));
            g.fillRoundedRectangle(juce::Rectangle<float>(b.getX()+3,y+1.5f,b.getWidth()-6,h-3.0f),1.8f);
        }
    }
    float level=0;
};

class PerfectRectangleVU : public juce::Component
{
public:
    void setLevels(float v,float){vu=vu*0.68f+v*0.32f;repaint();}
    void paint(juce::Graphics& g) override
    {
        auto bounds=getLocalBounds().toFloat(); g.setColour(juce::Colour(0xff0A0A0A)); g.fillRoundedRectangle(bounds,7.0f); g.setColour(juce::Colour(0xff3A3A3A)); g.drawRoundedRectangle(bounds,7.0f,1.2f);
        auto inner=bounds.reduced(8.0f);
        juce::ColourGradient warm(juce::Colour(0xffFFF1A0),inner.getX(),inner.getY(),juce::Colour(0xffFF9E1A),inner.getX(),inner.getBottom(),false);
        warm.addColour(0.25,juce::Colour(0xffFFE86A)); warm.addColour(0.50,juce::Colour(0xffFFCC30)); warm.addColour(0.75,juce::Colour(0xffFFB020));
        g.setGradientFill(warm); g.fillRoundedRectangle(inner,3.5f);
        float px=inner.getCentreX(); float py=inner.getBottom()+55.0f; float radius=inner.getWidth()*0.38f;
        struct Anchor{float db; float angleDeg;}; Anchor anchors[]={{-20.0f,-48.0f},{-10.0f,-32.0f},{-7.0f,-20.5f},{-5.0f,-12.0f},{-3.0f,-3.5f},{-2.0f,1.0f},{-1.0f,7.0f},{0.0f,16.0f},{1.0f,28.0f},{2.0f,41.0f},{3.0f,54.0f}};
        auto dbToAngle=[&](float db)->float{db=juce::jlimit(-20.0f,3.0f,db); for(int i=0;i<10;i++){if(db>=anchors[i].db&&db<=anchors[i+1].db){float t=(db-anchors[i].db)/(anchors[i+1].db-anchors[i].db); return juce::jmap(t,anchors[i].angleDeg,anchors[i+1].angleDeg);}} return 16.0f;};
        struct BottomAnchor{float db; float angleDeg; const char* label;}; BottomAnchor bottoms[]={{-20.0f,-48.0f,"0"},{-10.0f,-28.0f,"20"},{-7.0f,-17.0f,"40"},{-3.0f,-3.0f,"60"},{-2.0f,1.5f,"80"},{0.0f,16.0f,"100"}};
        juce::Path blackArc,redArc; bool firstB=true,firstR=true; for(float d=-20.0f;d<=0.0f;d+=0.1f){float a=dbToAngle(d)*juce::MathConstants<float>::pi/180.0f; float x=px+radius*std::sin(a); float y=py-radius*std::cos(a); if(firstB){blackArc.startNewSubPath(x,y);firstB=false;} else blackArc.lineTo(x,y);} for(float d=0.0f;d<=3.0f;d+=0.1f){float a=dbToAngle(d)*juce::MathConstants<float>::pi/180.0f; float x=px+radius*std::sin(a); float y=py-radius*std::cos(a); if(firstR){redArc.startNewSubPath(x,y);firstR=false;} else redArc.lineTo(x,y);} g.setColour(juce::Colour(0xff101010)); g.strokePath(blackArc,juce::PathStrokeType(1.8f)); g.setColour(juce::Colour(0xffE30613)); g.strokePath(redArc,juce::PathStrokeType(5.5f));
        for(auto anc:anchors){float a=anc.angleDeg*juce::MathConstants<float>::pi/180.0f; bool isRed=anc.db>=1.0f; float x1=px+radius*std::sin(a); float y1=py-radius*std::cos(a); float x2=px+(radius-14.0f)*std::sin(a); float y2=py-(radius-14.0f)*std::cos(a); g.setColour(isRed?juce::Colour(0xffCC0000):juce::Colour(0xff101010)); g.drawLine(x1,y1,x2,y2,1.6f); float rl=radius-28.0f; float lx=px+rl*std::sin(a); float ly=py-rl*std::cos(a); g.setFont(juce::Font(juce::FontOptions(anc.db==0?17.0f:13.0f,juce::Font::bold))); g.setColour(isRed?juce::Colour(0xffCC0000):juce::Colour(0xff101010)); juce::String txt=(anc.db>0?"+"+juce::String((int)anc.db):juce::String((int)anc.db)); if(anc.db==0)txt="0"; g.drawText(txt,(int)lx-15,(int)ly-16,30,18,juce::Justification::centred);}
        for(auto btm:bottoms){float a=btm.angleDeg*juce::MathConstants<float>::pi/180.0f; float rd=radius-14.0f; float rLabel=radius-32.0f; float dx=px+rd*std::sin(a); float dy=py-rd*std::cos(a); float lx=px+rLabel*std::sin(a); float ly=py-rLabel*std::cos(a); g.setColour(juce::Colour(0xff101010)); g.fillEllipse(dx-2.5f,dy-2.5f,5.0f,5.0f); g.setFont(juce::Font(juce::FontOptions(10.0f,juce::Font::bold))); g.setColour(juce::Colour(0xff202020).withAlpha(0.8f)); g.drawText(btm.label,(int)lx-10,(int)ly,20,12,juce::Justification::centred);}
        g.setFont(juce::Font(juce::FontOptions(11.0f,juce::Font::bold))); g.setColour(juce::Colour(0xff101010)); g.drawText("VU",inner.getX()+30,inner.getY()+14,40,14,juce::Justification::centred); g.setColour(juce::Colour(0xffCC0000)); g.drawText("VU",inner.getRight()-70,inner.getY()+14,40,14,juce::Justification::centred); g.setColour(juce::Colour(0xffC0C0C0)); g.fillEllipse(inner.getRight()-20,inner.getY()+12,10,10);
        float dbVU; if(vu<0.0003f)dbVU=-20.0f; else{dbVU=juce::Decibels::gainToDecibels(vu+0.00001f); dbVU=juce::jlimit(-20.0f,3.0f,dbVU);} float nDeg=dbToAngle(dbVU); float nRad=nDeg*juce::MathConstants<float>::pi/180.0f; float nx=px+(radius-2.0f)*std::sin(nRad); float ny=py-(radius-2.0f)*std::cos(nRad); g.setColour(juce::Colour(0xff0A0A0A)); g.drawLine(px,py,nx,ny,2.0f); g.fillEllipse(px-3.5f,py-3.5f,7,7);
    }
    float vu=0.0f;
};

class BigSeyaTitle : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        auto b = getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xffFFD060));
        juce::Font f(juce::FontOptions(52.0f, juce::Font::bold));
        f.setExtraKerningFactor(0.04f);
        g.setFont(f);
        g.drawText("SEYA MUKISA", b, juce::Justification::centredLeft, true);
        g.setColour(juce::Colour(0xff000000).withAlpha(0.35f));
        g.drawText("SEYA MUKISA", b.translated(1.5f,1.5f), juce::Justification::centredLeft, true);
    }
};

// ============ SINGLE FILE EDITOR - ALL IN ONE ============
class SeyaMukisaCompAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    SeyaMukisaCompAudioProcessorEditor(SeyaMukisaCompAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
    {
        setSize(1000,720);
        for(auto* k:{&threshKnob,&ratioKnob,&attackKnob,&releaseKnob,&kneeKnob,&makeupKnob})
        {k->setLookAndFeel(&knobLook);k->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
         k->setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);addAndMakeVisible(k);}
        addAndMakeVisible(mainVuMeter);addAndMakeVisible(inMeter);addAndMakeVisible(outMeter);
        addAndMakeVisible(titleLogo);addAndMakeVisible(compLabel);addAndMakeVisible(presetLabel);
        addAndMakeVisible(inLabel);addAndMakeVisible(outLabel);addAndMakeVisible(thL);addAndMakeVisible(raL);
        addAndMakeVisible(atL);addAndMakeVisible(relL);addAndMakeVisible(knL);addAndMakeVisible(maL);
        addAndMakeVisible(thV);addAndMakeVisible(raV);addAndMakeVisible(atV);addAndMakeVisible(relV);
        addAndMakeVisible(knV);addAndMakeVisible(maV);addAndMakeVisible(footerLabel);addAndMakeVisible(presetBox);

        compLabel.setText("",juce::dontSendNotification);
        compLabel.setColour(juce::Label::textColourId,juce::Colour(0xffFFAA40));
        compLabel.setFont(juce::Font(juce::FontOptions(14.0f,juce::Font::bold)));

        presetBox.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff2A1515));
        presetBox.setColour(juce::ComboBox::textColourId,juce::Colour(0xffFFD060));
        for(int i=0;i<audioProcessor.presets.size();++i) presetBox.addItem(audioProcessor.presets[i].name,i+1);
        presetBox.setSelectedId(1);
        presetBox.onChange=[this]{audioProcessor.applyPreset(presetBox.getSelectedId()-1);};

        auto make=[&](juce::Label& l,juce::String t, bool isVal=false){
            l.setText(t,juce::dontSendNotification); l.setJustificationType(juce::Justification::centred);
            l.setColour(juce::Label::textColourId,isVal? juce::Colour(0xffFFD8A0) : juce::Colour(0xffFFC060));
            l.setFont(juce::Font(juce::FontOptions(isVal?12.0f:14.0f,juce::Font::bold)));
        };
        make(inLabel,"INPUT"); make(outLabel,"OUTPUT");
        make(thL,"THRESHOLD"); make(raL,"RATIO"); make(atL,"ATTACK");
        make(relL,"RELEASE"); make(knL,"KNEE"); make(maL,"MAKEUP");
        make(thV,"-24.2 dB",true); make(raV,"6:1",true); make(atV,"12 ms",true);
        make(relV,"85 ms",true); make(knV,"4.0 dB",true); make(maV,"+3.4 dB",true);
        make(footerLabel,"SEYA MUKISA COMPRESSOR v1.0 | RED EDITION - SINGLE FILE");

        a1.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"thresh",threshKnob));
        a2.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"ratio",ratioKnob));
        a3.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"attack",attackKnob));
        a4.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"release",releaseKnob));
        a5.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"knee",kneeKnob));
        a6.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"makeup",makeupKnob));
        startTimerHz(30);
    }

    ~SeyaMukisaCompAudioProcessorEditor() override
    {
        stopTimer();
        for(auto* k:{&threshKnob,&ratioKnob,&attackKnob,&releaseKnob,&kneeKnob,&makeupKnob})
            k->setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {
        auto b=getLocalBounds().toFloat();
        g.setColour(juce::Colour(0xff1A0E12)); g.fillAll();
        juce::ColourGradient pg(juce::Colour(0xff3A1A20),b.getCentreX(),b.getY(),juce::Colour(0xff12080A),b.getCentreX(),b.getBottom(),false);
        pg.addColour(0.45,juce::Colour(0xff2A1218)); g.setGradientFill(pg); g.fillAll();
        auto top=juce::Rectangle<float>(5,5,b.getWidth()-10,60); g.setColour(juce::Colour(0xff2E1A20)); g.fillRoundedRectangle(top,7);
        auto vuArea=juce::Rectangle<float>(16,76,968,228); g.setColour(juce::Colour(0xff1E1216)); g.fillRoundedRectangle(vuArea,8);
        auto bot=juce::Rectangle<float>(10,322,980,340); g.setColour(juce::Colour(0xff221118)); g.fillRoundedRectangle(bot,8);
        auto foot=juce::Rectangle<float>(10,674,980,38); g.setColour(juce::Colour(0xff140A0E)); g.fillRoundedRectangle(foot,6);
    }

    void resized() override
    {
        titleLogo.setBounds(18,2,560,58);
        compLabel.setBounds(24,44,200,14);
        presetLabel.setBounds(680,8,80,14); presetBox.setBounds(680,24,300,34);
        inLabel.setBounds(22,86,130,18); inMeter.setBounds(30,108,50,200);
        outLabel.setBounds(848,86,130,18); outMeter.setBounds(866,108,50,200);
        mainVuMeter.setBounds(192,86,616,208);
        int yL=360; int yK=384; int yV=526;
        thL.setBounds(18,yL,152,18); threshKnob.setBounds(28,yK,132,132); thV.setBounds(18,yV,152,20);
        raL.setBounds(184,yL,142,18); ratioKnob.setBounds(190,yK,124,124); raV.setBounds(184,yV,142,20);
        atL.setBounds(350,yL,142,18); attackKnob.setBounds(356,yK,124,124); atV.setBounds(350,yV,142,20);
        relL.setBounds(508,yL,142,18); releaseKnob.setBounds(514,yK,124,124); relV.setBounds(508,yV,142,20);
        knL.setBounds(672,yL,142,18); kneeKnob.setBounds(678,yK,124,124); knV.setBounds(672,yV,142,20);
        maL.setBounds(832,yL,152,18); makeupKnob.setBounds(842,yK,132,132); maV.setBounds(832,yV,152,20);
        footerLabel.setBounds(10,674,980,38);
    }

    void timerCallback() override
    {
        float inL=audioProcessor.inLevel.load(); float outL=audioProcessor.outLevel.load(); float vu=audioProcessor.vuLevel.load();
        mainVuMeter.setLevels(vu,0); inMeter.setLevel(inL); outMeter.setLevel(outL);
        thV.setText(juce::String(audioProcessor.apvts.getRawParameterValue("thresh")->load(),1)+" dB",juce::dontSendNotification);
        raV.setText(juce::String((int)audioProcessor.apvts.getRawParameterValue("ratio")->load())+":1",juce::dontSendNotification);
        atV.setText(juce::String((int)audioProcessor.apvts.getRawParameterValue("attack")->load())+" ms",juce::dontSendNotification);
        relV.setText(juce::String((int)audioProcessor.apvts.getRawParameterValue("release")->load())+" ms",juce::dontSendNotification);
        knV.setText(juce::String(audioProcessor.apvts.getRawParameterValue("knee")->load(),1)+" dB",juce::dontSendNotification);
        maV.setText((audioProcessor.apvts.getRawParameterValue("makeup")->load()>=0?"+":"")+juce::String(audioProcessor.apvts.getRawParameterValue("makeup")->load(),1)+" dB",juce::dontSendNotification);
    }

private:
    SeyaMukisaCompAudioProcessor& audioProcessor;
    GreenDarkRedActionLook knobLook;
    juce::Slider threshKnob,ratioKnob,attackKnob,releaseKnob,kneeKnob,makeupKnob;
    PerfectRectangleVU mainVuMeter; OrangeLedMeter inMeter,outMeter;
    BigSeyaTitle titleLogo;
    juce::Label compLabel,presetLabel,inLabel,outLabel,thL,raL,atL,relL,knL,maL,thV,raV,atV,relV,knV,maV,footerLabel;
    juce::ComboBox presetBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> a1,a2,a3,a4,a5,a6;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeyaMukisaCompAudioProcessorEditor)
};
