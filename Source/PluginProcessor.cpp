#include "PluginProcessor.h"
#include "PluginEditor.h"
SeyaMukisaCompAudioProcessor::SeyaMukisaCompAudioProcessor():AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)),apvts(*this,nullptr,"PARAMS",createParams()){
presets={{"Smooth R&B Clean",-18.5f,4.0f,12.0f,85.0f,4.0f,3.4f,-8.3f,3.2f},{"Vocal Warm",-14.0f,3.0f,8.0f,120.0f,6.0f,4.5f,-6.0f,2.0f},{"Tight Punch",-12.0f,6.0f,2.0f,50.0f,1.5f,2.0f,-4.0f,4.0f},{"Master Glue",-8.0f,2.0f,30.0f,300.0f,8.0f,1.5f,-2.0f,1.0f},{"Bass Heavy",-16.0f,5.0f,15.0f,150.0f,3.0f,5.0f,-5.0f,3.5f},{"Drum Smash",-20.0f,10.0f,1.0f,80.0f,0.5f,6.0f,-3.0f,5.0f},{"Soft Lofi",-22.0f,2.5f,25.0f,400.0f,9.0f,2.0f,-10.0f,0.0f},{"Bright Air",-10.0f,3.5f,5.0f,60.0f,5.0f,3.0f,-7.0f,2.5f},{"Deep Soul",-15.0f,4.5f,18.0f,200.0f,4.5f,4.0f,-9.0f,3.0f},{"Hard Limit",-6.0f,20.0f,0.5f,30.0f,0.0f,0.0f,0.0f,6.0f},{"RNB Adlib Snap",-13.0f,8.0f,3.0f,40.0f,2.0f,5.5f,-4.5f,4.5f},{"Warm Tape",-17.0f,3.2f,20.0f,250.0f,7.0f,2.8f,-8.0f,1.8f}};}
SeyaMukisaCompAudioProcessor::~SeyaMukisaCompAudioProcessor(){}
juce::AudioProcessorValueTreeState::ParameterLayout SeyaMukisaCompAudioProcessor::createParams(){std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"thresh",1},"Threshold",juce::NormalisableRange<float>(-40,0),-18.5f));
p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"ratio",1},"Ratio",juce::NormalisableRange<float>(1,20),4.0f));
p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"attack",1},"Attack",juce::NormalisableRange<float>(0.1f,100.0f),12.0f));
p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"release",1},"Release",juce::NormalisableRange<float>(10,1000),85.0f));
p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"knee",1},"Knee",juce::NormalisableRange<float>(0,10),4.0f));
p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"makeup",1},"Makeup",juce::NormalisableRange<float>(-12,12),3.4f));
p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"input",1},"Input",juce::NormalisableRange<float>(-24,12),-8.3f));
p.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"output",1},"Output",juce::NormalisableRange<float>(-24,12),3.2f));
return {p.begin(),p.end()};}
void SeyaMukisaCompAudioProcessor::applyPreset(int idx){if(idx<0||idx>=presets.size())return;currentPreset=idx;auto& ps=presets[idx];
apvts.getParameter("thresh")->setValueNotifyingHost(apvts.getParameter("thresh")->convertTo0to1(ps.thresh));
apvts.getParameter("ratio")->setValueNotifyingHost(apvts.getParameter("ratio")->convertTo0to1(ps.ratio));
apvts.getParameter("attack")->setValueNotifyingHost(apvts.getParameter("attack")->convertTo0to1(ps.attack));
apvts.getParameter("release")->setValueNotifyingHost(apvts.getParameter("release")->convertTo0to1(ps.release));
apvts.getParameter("knee")->setValueNotifyingHost(apvts.getParameter("knee")->convertTo0to1(ps.knee));
apvts.getParameter("makeup")->setValueNotifyingHost(apvts.getParameter("makeup")->convertTo0to1(ps.makeup));
apvts.getParameter("input")->setValueNotifyingHost(apvts.getParameter("input")->convertTo0to1(ps.input));
apvts.getParameter("output")->setValueNotifyingHost(apvts.getParameter("output")->convertTo0to1(ps.output));}
void SeyaMukisaCompAudioProcessor::setCurrentProgram(int idx){applyPreset(idx);}
void SeyaMukisaCompAudioProcessor::prepareToPlay(double,int){}void SeyaMukisaCompAudioProcessor::releaseResources(){}
bool SeyaMukisaCompAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const{return l.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();}
void SeyaMukisaCompAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,juce::MidiBuffer&){juce::ScopedNoDenormals noDenormals;
auto thresh=apvts.getRawParameterValue("thresh")->load();auto ratio=apvts.getRawParameterValue("ratio")->load();
auto makeup=juce::Decibels::decibelsToGain(apvts.getRawParameterValue("makeup")->load());
auto inGain=juce::Decibels::decibelsToGain(apvts.getRawParameterValue("input")->load());
auto outGain=juce::Decibels::decibelsToGain(apvts.getRawParameterValue("output")->load());
float inPeak=0,outPeak=0,sum=0,maxGR=0;
for(int ch=0;ch<buffer.getNumChannels();++ch){auto* d=buffer.getWritePointer(ch);for(int i=0;i<buffer.getNumSamples();++i){float s=d[i]*inGain;inPeak=juce::jmax(inPeak,std::abs(s));float db=juce::Decibels::gainToDecibels(std::abs(s)+0.00001f);float over=db-thresh;if(over>0){float gr=over*(1.0f-1.0f/ratio);envelope=envelope*0.85f+gr*0.15f;}else envelope*=0.998f;maxGR=juce::jmax(maxGR,envelope);s*=juce::Decibels::decibelsToGain(-envelope);s*=makeup*outGain;sum+=s*s;outPeak=juce::jmax(outPeak,std::abs(s));d[i]=s;}}
float rms=std::sqrt(sum/(buffer.getNumSamples()*buffer.getNumChannels()+1));if(inPeak>0.0001f||outPeak>0.0001f){inLevel.store(inPeak);outLevel.store(outPeak);vuLevel.store(rms);grLevel.store(maxGR);}else{inLevel.store(inLevel.load()*0.92f);outLevel.store(outLevel.load()*0.92f);vuLevel.store(vuLevel.load()*0.92f);if(grLevel.load()<0.1f)grLevel.store(0);}}
juce::AudioProcessorEditor* SeyaMukisaCompAudioProcessor::createEditor(){return new SeyaMukisaCompAudioProcessorEditor(*this);}
void SeyaMukisaCompAudioProcessor::getStateInformation(juce::MemoryBlock& d){auto s=apvts.copyState();std::unique_ptr<juce::XmlElement> x(s.createXml());copyXmlToBinary(*x,d);}
void SeyaMukisaCompAudioProcessor::setStateInformation(const void* d,int sz){std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(d,sz));if(x!=nullptr)if(x->hasTagName(apvts.state.getType()))apvts.replaceState(juce::ValueTree::fromXml(*x));}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new SeyaMukisaCompAudioProcessor();}
