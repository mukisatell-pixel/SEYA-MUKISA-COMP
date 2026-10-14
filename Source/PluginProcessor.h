#pragma once
#include <JuceHeader.h>
struct CompressorPreset{juce::String name; float thresh,ratio,attack,release,knee,makeup,input,output;};
class SeyaMukisaCompAudioProcessor : public juce::AudioProcessor{
public:
SeyaMukisaCompAudioProcessor();~SeyaMukisaCompAudioProcessor() override;
void prepareToPlay(double sampleRate,int samplesPerBlock) override;void releaseResources() override;
bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
juce::AudioProcessorEditor* createEditor() override;bool hasEditor() const override{return true;}
const juce::String getName() const override{return "SEYA MUKISA COMP";}bool acceptsMidi() const override{return false;}bool producesMidi() const override{return false;}bool isMidiEffect() const override{return false;}double getTailLengthSeconds() const override{return 0.0;}
int getNumPrograms() override{return (int)presets.size();}int getCurrentProgram() override{return currentPreset;}void setCurrentProgram(int index) override;
const juce::String getProgramName(int index) override{return presets[index].name;}void changeProgramName(int,const juce::String&) override{}
void getStateInformation(juce::MemoryBlock& destData) override;void setStateInformation(const void* data,int sizeInBytes) override;
juce::AudioProcessorValueTreeState apvts;std::atomic<float> vuLevel{0},grLevel{0},inLevel{0},outLevel{0};std::vector<CompressorPreset> presets;int currentPreset=0;void applyPreset(int index);
private:juce::AudioProcessorValueTreeState::ParameterLayout createParams();float envelope=0;
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SeyaMukisaCompAudioProcessor)};
