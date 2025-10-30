#include "BpmStream.h"
#include <algorithm>
#include <iostream>
#include <vector>

BpmStream::BpmStream()
    : m_bpmDetector(44100.0f)
{
    m_floatBuffer.resize(8192);
}

bool BpmStream::load(const std::string& filename)
{
    if (!m_buffer.loadFromFile(filename))
    {
        std::cerr << "Failed to load file: " << filename << std::endl;
        return false;
    }

    m_samples = m_buffer.getSamples();
    m_totalSamples = m_buffer.getSampleCount();
    m_offset = 0;
    m_chunkCounter = 0;

    unsigned int channelCount = m_buffer.getChannelCount();
    unsigned int sampleRate = m_buffer.getSampleRate();

    std::cout << "Audio loaded - Rate: " << sampleRate
        << " Hz, Channels: " << channelCount
        << ", Samples: " << m_totalSamples << std::endl;
    std::vector<sf::SoundChannel> channelMap;
    if (channelCount == 1)
    {
        channelMap.push_back(sf::SoundChannel::Mono);
    }
    else if (channelCount == 2)
    {
        channelMap.push_back(sf::SoundChannel::FrontLeft);
        channelMap.push_back(sf::SoundChannel::FrontRight);
    }
    else
    {
        std::cerr << "Unsupported channel count: " << channelCount << std::endl;
        return false;
    }
    SoundStream::initialize(channelCount, sampleRate, channelMap);

    return true;
}

void BpmStream::analyzeBPM()
{

    std::cout << "Analyzing BPM... (this may take a moment)" << std::endl;

    // sample size ( first 30 seconds)
    unsigned int sampleRate = m_buffer.getSampleRate();
    unsigned int channelCount = m_buffer.getChannelCount();
    size_t samplesPerSecond = sampleRate * channelCount;
    size_t samplesToAnalyze = std::min(m_totalSamples, samplesPerSecond * 30); // 30 seconds max

    // Convert samples to float
    std::vector<float> floatSamples(samplesToAnalyze);
    for (size_t i = 0; i < samplesToAnalyze; ++i)
    {
        floatSamples[i] = m_samples[i] / 32768.0f;
    }

    // Analyze the entire chunk at once for BPM
    m_currentBpm = m_bpmDetector.estimateTempoOfSamples(floatSamples.data(), static_cast<int>(samplesToAnalyze));
    std::cout << "BPM Analysis complete: " << m_currentBpm << " BPM" << std::endl;

    //multiple candidates to show confidence
    auto candidates = m_bpmDetector.getTopCandidates(3);
    if (!candidates.empty())
    {
        std::cout << "Top BPM candidates:" << std::endl;
        for (const auto& candidate : candidates)
        {
            std::cout << "  " << candidate.bpm << " BPM (confidence: " << candidate.confidence << ")" << std::endl;
        }
    }
}

double BpmStream::getCurrentBPM() const
{
    return m_currentBpm;
}

bool BpmStream::onGetData(Chunk& data)
{
    constexpr int CHUNK_SIZE = 8192;

    if (m_offset >= m_totalSamples)
        return false;

    size_t remaining = std::min<size_t>(CHUNK_SIZE, m_totalSamples - m_offset);
    data.samples = m_samples + m_offset;
    data.sampleCount = remaining;

    // Only update BPM every 30 chunks (~0.5 seconds) to reduce CPU load
    if (++m_chunkCounter >= 30)
    {
        m_chunkCounter = 0;

        if (m_floatBuffer.size() < remaining)
            m_floatBuffer.resize(remaining);
        for (size_t i = 0; i < remaining; ++i)
            m_floatBuffer[i] = m_samples[m_offset + i] / 32768.0f;
        m_bpmDetector.process(m_floatBuffer.data(), static_cast<int>(remaining));
        m_currentBpm = m_bpmDetector.estimateTempo();
    }

    m_offset += remaining;
    return true;
}

void BpmStream::onSeek(sf::Time timeOffset)
{
    m_offset = static_cast<size_t>(timeOffset.asSeconds() * m_buffer.getSampleRate() * m_buffer.getChannelCount());
}