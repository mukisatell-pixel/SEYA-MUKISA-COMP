#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class GreenDarkRedActionLook : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos, float start, float end, juce::Slider&) override
    {
        auto b = juce::Rectangle<float>((float)x,(float)y,(float)w,(float)h).reduced(2.0f);
        auto c = b.getCentre(); float r = b.getWidth()*0.5f; float ang = start + pos*(end-start);
        g.setColour(juce::Colour(0xff0A120C)); g.fillEllipse(c.x-r+1,c.y-r+3,r*2,r*2);
        g.setColour(juce::Colour(0xff3AA0FF)); g.drawEllipse(c.x-r,c.y-r,r*2,r*2,4.5f);
        juce::Path ring; ring.addCentredArc(c.x,c.y,r-3,r-3,0,start,ang,true);
        g.setColour(juce::Colour(0xffCC1225).withAlpha(0.92f)); g.strokePath(ring,juce::PathStrokeType(5,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
        g.setColour(juce::Colour(0xffFF2040).withAlpha(0.35f+pos*0.35f)); g.strokePath(ring,juce::PathStrokeType(9.5f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
        auto inner=b.reduced(12); g.setColour(juce::Colour(0xff060606)); g.fillEllipse(inner);
        bool isBig=w>125; juce::Colour tick=isBig?juce::Colour(0xff00FF55):juce::Colour(0xffFF8A20); float len=isBig?r*0.34f:r*0.24f;
        juce::Point<float> p1(c.x+r*0.10f*std::cos(ang-1.5708f),c.y+r*0.10f*std::sin(ang-1.5708f));
        juce::Point<float> p2(c.x+len*std::cos(ang-1.5708f),c.y+len*std::sin(ang-1.5708f));
        g.setColour(tick); g.drawLine(p1.x,p1.y,p2.x,p2.y,isBig?1.9f:1.3f);
    }
};

class OrangeLedMeter : public juce::Component{
public: void setLevel(float l){level=level*0.55f+l*0.45f;repaint();}
void paint(juce::Graphics& g) override{
auto b=getLocalBounds().toFloat(); g.setColour(juce::Colour(0xff0A0A0A)); g.fillRoundedRectangle(b,4);
float segs[]={8,6,4,2,0,-2,-4,-8,-12,-16,-20,-28,-40,-60}; int n=14; float h=(b.getHeight()-8)/n; float db=juce::Decibels::gainToDecibels(level+0.0001f);
for(int i=0;i<n;i++){float y=b.getY()+4+i*h; bool on=db>segs[i]; juce::Colour col=(i==1)?juce::Colour(0xffFF5533):juce::Colour(0xffFFA530); if(on){g.setColour(col.withAlpha(0.4f)); g.fillRoundedRectangle(b.getX()+2,y+1,b.getWidth()-4,h-1.5f,1.5f);} g.setColour(on?col:juce::Colour(0xff1A1A1A).withAlpha(0.6f)); g.fillRoundedRectangle(juce::Rectangle<float>(b.getX()+3,y+1.5f,b.getWidth()-6,h-3),1.8f);} } float level=0;};

class PerfectRectangleVU : public juce::Component{
public: void setLevels(float v,float){vu=vu*0.68f+v*0.32f;repaint();}
void paint(juce::Graphics& g) override{
auto bounds=getLocalBounds().toFloat(); g.setColour(juce::Colour(0xff0A0A0A)); g.fillRoundedRectangle(bounds,7); g.setColour(juce::Colour(0xff3A3A3A)); g.drawRoundedRectangle(bounds,7,1.2f);
auto inner=bounds.reduced(8);
juce::ColourGradient warm(juce::Colour(0xffFFF1A0),inner.getX(),inner.getY(),juce::Colour(0xffFF9E1A),inner.getX(),inner.getBottom(),false);
warm.addColour(0.25,juce::Colour(0xffFFE86A)); warm.addColour(0.50,juce::Colour(0xffFFCC30)); warm.addColour(0.75,juce::Colour(0xffFFB020));
g.setGradientFill(warm); g.fillRoundedRectangle(inner,3.5f);
float px=inner.getCentreX(); float py=inner.getBottom()+18.0f; float radius=inner.getWidth()*0.48f;
struct Anchor{float db;float angleDeg;}; Anchor anchors[]={{-20.0f,-48.0f},{-10.0f,-32.0f},{-7.0f,-20.5f},{-5.0f,-12.0f},{-3.0f,-3.5f},{-2.0f,1.0f},{-1.0f,7.0f},{0.0f,16.0f},{1.0f,28.0f},{2.0f,41.0f},{3.0f,54.0f}};
auto dbToAngle=[&](float db)->float{db=juce::jlimit(-20.0f,3.0f,db); for(int i=0;i<10;i++){if(db>=anchors[i].db&&db<=anchors[i+1].db){float t=(db-anchors[i].db)/(anchors[i+1].db-anchors[i].db); return juce::jmap(t,anchors[i].angleDeg,anchors[i+1].angleDeg);}} return 16.0f;};
struct BottomAnchor{float db;float angleDeg;const char* label;}; BottomAnchor bottoms[]={{-20.0f,-48.0f,"0"},{-10.0f,-28.0f,"20"},{-7.0f,-17.0f,"40"},{-3.0f,-3.0f,"60"},{-2.0f,1.5f,"80"},{0.0f,16.0f,"100"}};
juce::Path blackArc,redArc; bool firstB=true,firstR=true; for(float d=-20;d<=0;d+=0.1f){float a=dbToAngle(d)*3.14159f/180; float x=px+radius*std::sin(a); float y=py-radius*std::cos(a); if(firstB){blackArc.startNewSubPath(x,y);firstB=false;} else blackArc.lineTo(x,y);} for(float d=0;d<=3;d+=0.1f){float a=dbToAngle(d)*3.14159f/180; float x=px+radius*std::sin(a); float y=py-radius*std::cos(a); if(firstR){redArc.startNewSubPath(x,y);firstR=false;} else redArc.lineTo(x,y);} g.setColour(juce::Colour(0xff101010)); g.strokePath(blackArc,juce::PathStrokeType(1.8f)); g.setColour(juce::Colour(0xffE30613)); g.strokePath(redArc,juce::PathStrokeType(5.5f));
for(auto anc:anchors){float a=anc.angleDeg*3.14159f/180; bool isRed=anc.db>=1; float x1=px+radius*std::sin(a); float y1=py-radius*std::cos(a); float x2=px+(radius-14)*std::sin(a); float y2=py-(radius-14)*std::cos(a); g.setColour(isRed?juce::Colour(0xffCC0000):juce::Colour(0xff101010)); g.drawLine(x1,y1,x2,y2,1.6f); float rl=radius-28; float lx=px+rl*std::sin(a); float ly=py-rl*std::cos(a); g.setFont(juce::Font(juce::FontOptions(anc.db==0?17:13,juce::Font::bold))); g.setColour(isRed?juce::Colour(0xffCC0000):juce::Colour(0xff101010)); juce::String txt=(anc.db>0?"+"+juce::String((int)anc.db):juce::String((int)anc.db)); if(anc.db==0)txt="0"; g.drawText(txt,(int)lx-15,(int)ly-16,30,18,juce::Justification::centred);}
for(auto btm:bottoms){float a=btm.angleDeg*3.14159f/180; float rd=radius-14; float rLabel=radius-32; float dx=px+rd*std::sin(a); float dy=py-rd*std::cos(a); float lx=px+rLabel*std::sin(a); float ly=py-rLabel*std::cos(a); g.setColour(juce::Colour(0xff101010)); g.fillEllipse(dx-2.5f,dy-2.5f,5,5); g.setFont(juce::Font(juce::FontOptions(10,juce::Font::bold))); g.setColour(juce::Colour(0xff202020).withAlpha(0.8f)); g.drawText(btm.label,(int)lx-10,(int)ly,20,12,juce::Justification::centred);}
g.setFont(juce::Font(juce::FontOptions(11,juce::Font::bold))); g.setColour(juce::Colour(0xff101010)); g.drawText("VU",inner.getX()+30,inner.getY()+14,40,14,juce::Justification::centred); g.setColour(juce::Colour(0xffCC0000)); g.drawText("VU",inner.getRight()-70,inner.getY()+14,40,14,juce::Justification::centred); g.setColour(juce::Colour(0xffC0C0C0)); g.fillEllipse(inner.getRight()-20,inner.getY()+12,10,10);
float dbVU; if(vu<0.0003f)dbVU=-20; else{dbVU=juce::Decibels::gainToDecibels(vu+0.00001f); dbVU=juce::jlimit(-20.f,3.f,dbVU);} float nDeg=dbToAngle(dbVU); float nRad=nDeg*3.14159f/180; float nx=px+(radius-2)*std::sin(nRad); float ny=py-(radius-2)*std::cos(nRad); g.setColour(juce::Colour(0xff0A0A0A)); g.drawLine(px,py,nx,ny,2); g.fillEllipse(px-3.5f,py-3.5f,7,7);} float vu=0;};

// COMPRESSOR REMOVED - ONLY SEYA MUKISA
class BigSeyaTitle : public juce::Component{
public: void paint(juce::Graphics& g) override{
g.setColour(juce::Colour(0xffFFD060));
g.setFont(juce::Font(juce::FontOptions(38,juce::Font::bold)));
g.drawText("SEYA MUKISA",getLocalBounds(),juce::Justification::centredLeft,false);
}};

class SeyaMukisaCompAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Timer{
public: SeyaMukisaCompAudioProcessorEditor(SeyaMukisaCompAudioProcessor& p):AudioProcessorEditor(&p),audioProcessor(p){
setSize(1000,720);
for(auto* k:{&threshKnob,&ratioKnob,&attackKnob,&releaseKnob,&kneeKnob,&makeupKnob}){k->setLookAndFeel(&knobLook); k->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); k->setTextBoxStyle(juce::Slider::NoTextBox,false,0,0); addAndMakeVisible(k);}
addAndMakeVisible(mainVuMeter); addAndMakeVisible(inMeter); addAndMakeVisible(outMeter); addAndMakeVisible(titleLogo); addAndMakeVisible(inLabel); addAndMakeVisible(outLabel); addAndMakeVisible(thL); addAndMakeVisible(raL); addAndMakeVisible(atL); addAndMakeVisible(relL); addAndMakeVisible(knL); addAndMakeVisible(maL); addAndMakeVisible(thV); addAndMakeVisible(raV); addAndMakeVisible(atV); addAndMakeVisible(relV); addAndMakeVisible(knV); addAndMakeVisible(maV); addAndMakeVisible(footerLabel); addAndMakeVisible(presetBox);
presetBox.setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff2A1515)); presetBox.setColour(juce::ComboBox::textColourId,juce::Colour(0xffFFD060));
for(int i=0;i<audioProcessor.presets.size();++i) presetBox.addItem(audioProcessor.presets[i].name,i+1); presetBox.setSelectedId(1); presetBox.onChange=[this]{audioProcessor.applyPreset(presetBox.getSelectedId()-1);};
auto make=[&](juce::Label& l,juce::String t,bool isVal=false){l.setText(t,juce::dontSendNotification); l.setJustificationType(juce::Justification::centred); l.setColour(juce::Label::textColourId,isVal?juce::Colour(0xffFFD8A0):juce::Colour(0xffFFC060)); l.setFont(juce::Font(juce::FontOptions(isVal?12:14,juce::Font::bold)));};
make(inLabel,"INPUT"); make(outLabel,"OUTPUT"); make(thL,"THRESHOLD"); make(raL,"RATIO"); make(atL,"ATTACK"); make(relL,"RELEASE"); make(knL,"KNEE"); make(maL,"MAKEUP");
make(thV,"-18.5 dB",true); make(raV,"4:1",true); make(atV,"12 ms",true); make(relV,"85 ms",true); make(knV,"4.0 dB",true); make(maV,"+3.4 dB",true);
make(footerLabel,"SEYA MUKISA v1.0 | RED EDITION - LOCKED");
a1.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"thresh",threshKnob)); a2.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"ratio",ratioKnob)); a3.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"attack",attackKnob)); a4.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"release",releaseKnob)); a5.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"knee",kneeKnob)); a6.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts,"makeup",makeupKnob)); startTimerHz(30); }
~SeyaMukisaCompAudioProcessorEditor() override{stopTimer(); for(auto* k:{&threshKnob,&ratioKnob,&attackKnob,&releaseKnob,&kneeKnob,&makeupKnob}) k->setLookAndFeel(nullptr);}
void paint(juce::Graphics& g) override{auto b=getLocalBounds().toFloat(); g.setColour(juce::Colour(0xff1A0E12)); g.fillAll(); juce::ColourGradient pg(juce::Colour(0xff3A1A20),b.getCentreX(),b.getY(),juce::Colour(0xff12080A),b.getCentreX(),b.getBottom(),false); pg.addColour(0.45,juce::Colour(0xff2A1218)); g.setGradientFill(pg); g.fillAll(); auto top=juce::Rectangle<float>(5,5,b.getWidth()-10,68); g.setColour(juce::Colour(0xff2E1A20)); g.fillRoundedRectangle(top,8); auto vuArea=juce::Rectangle<float>(16,84,968,228); g.setColour(juce::Colour(0xff1E1216)); g.fillRoundedRectangle(vuArea,8); auto bot=juce::Rectangle<float>(10,330,980,332); g.setColour(juce::Colour(0xff221118)); g.fillRoundedRectangle(bot,8); auto foot=juce::Rectangle<float>(10,674,980,38); g.setColour(juce::Colour(0xff140A0E)); g.fillRoundedRectangle(foot,6); }
void resized() override{
titleLogo.setBounds(18,6,400,44);
presetBox.setBounds(680,24,300,34);
inLabel.setBounds(22,94,130,18); inMeter.setBounds(30,116,50,200);
outLabel.setBounds(848,94,130,18); outMeter.setBounds(866,116,50,200);
mainVuMeter.setBounds(192,94,616,208);
int yL=368,yK=392,yV=534;
thL.setBounds(18,yL,152,18); threshKnob.setBounds(28,yK,132,132); thV.setBounds(18,yV,152,20);
raL.setBounds(184,yL,142,18); ratioKnob.setBounds(190,yK,124,124); raV.setBounds(184,yV,142,20);
atL.setBounds(350,yL,142,18); attackKnob.setBounds(356,yK,124,124); atV.setBounds(350,yV,142,20);
relL.setBounds(508,yL,142,18); releaseKnob.setBounds(514,yK,124,124); relV.setBounds(508,yV,142,20);
knL.setBounds(672,yL,142,18); kneeKnob.setBounds(678,yK,124,124); knV.setBounds(672,yV,142,20);
maL.setBounds(832,yL,152,18); makeupKnob.setBounds(842,yK,132,132); maV.setBounds(832,yV,152,20);
footerLabel.setBounds(10,674,980,38); }
void timerCallback() override{mainVuMeter.setLevels(audioProcessor.vuLevel.load(),0); inMeter.setLevel(audioProcessor.inLevel.load()); outMeter.setLevel(audioProcessor.outLevel.load());
thV.setText(juce::String(audioProcessor.apvts.getRawParameterValue("thresh")->load(),1)+" dB",juce::dontSendNotification); raV.setText(juce::String((int)audioProcessor.apvts.getRawParameterValue("ratio")->load())+":1",juce::dontSendNotification); atV.setText(juce::String((int)audioProcessor.apvts.getRawParameterValue("attack")->load())+" ms",juce::dontSendNotification); relV.setText(juce::String((int)audioProcessor.apvts.getRawParameterValue("release")->load())+" ms",juce::dontSendNotification); knV.setText(juce::String(audioProcessor.apvts.getRawParameterValue("knee")->load(),1)+" dB",juce::dontSendNotification); maV.setText((audioProcessor.apvts.getRawParameterValue("makeup")->load()>=0?"+":"")+juce::String(audioProcessor.apvts.getRawParameterValue("makeup")->load(),1)+" dB",juce::dontSendNotification); }
private: SeyaMukisaCompAudioProcessor& audioProcessor; GreenDarkRedActionLook knobLook; juce::Slider threshKnob,ratioKnob,attackKnob,releaseKnob,kneeKnob,makeupKnob; PerfectRectangleVU mainVuMeter; OrangeLedMeter inMeter,outMeter; BigSeyaTitle titleLogo; juce::Label inLabel,outLabel,thL,raL,atL,relL,knL,maL,thV,raV,atV,relV,knV,maV,footerLabel; juce::ComboBox presetBox; std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> a1,a2,a3,a4,a5,a6; JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeyaMukisaCompAudioProcessorEditor) };

