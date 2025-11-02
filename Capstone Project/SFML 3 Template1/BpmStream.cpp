#include "BpmStream.h"
#include <algorithm>
#include <iostream>
#include <vector>

BpmStream::BpmStream()
    : m_bpmDetector(44100.0f)
{
    m_floatBuffer.resize(8192);
    m_bpmDetector.setBPMRange(60.0, 180.0);
}

bool BpmStream::load(const std::string& filename)
{
    stop();

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

    m_bpmDetector.reset();
    m_currentBpm = 0.0;

    analyzeBPM();


    return true;
}

void BpmStream::analyzeBPM()
{
    std::cout << "Analyzing BPM... (this may take a moment)" << std::endl;
    unsigned int sampleRate = m_buffer.getSampleRate();
    unsigned int channelCount = m_buffer.getChannelCount();
    size_t samplesPerSecond = sampleRate * channelCount;
    size_t samplesToAnalyze = std::min(m_totalSamples, samplesPerSecond * 30);

    std::vector<float> floatSamples(samplesToAnalyze);
    for (size_t i = 0; i < samplesToAnalyze; ++i)
    {
        floatSamples[i] = m_samples[i] / 32768.0f;
    }
    m_currentBpm = m_bpmDetector.estimateTempoOfSamples(floatSamples.data(), static_cast<int>(samplesToAnalyze));
    auto candidates = m_bpmDetector.getTempoCandidates();

    std::cout << "All BPM candidates:" << std::endl;
    for (size_t i = 0; i < candidates.size() && i < 5; ++i)
    {
        std::cout << "  Candidate " << i + 1 << ": " << candidates[i] << " BPM" << std::endl;
    }

    //top BPM is above 140
    if (m_currentBpm > 140.0 && candidates.size() >= 2)
    {
        double halfBpm = m_currentBpm / 2.0;
        // Check if the SECOND candidate is close to half (within 2 BPM)
        if (std::abs(candidates[1] - halfBpm) < 2.0)
        {
            std::cout << "Detected double-time! Using half BPM: " << candidates[1] << std::endl;
            m_currentBpm = candidates[1];
        }
        else
        {
            std::cout << "Keeping original BPM: " << m_currentBpm << std::endl;
        }
    }
    else
    {
        std::cout << "Using detected BPM: " << m_currentBpm << std::endl;
    }
    std::cout << "Final BPM: " << m_currentBpm << " BPM" << std::endl;
}

double BpmStream::getCurrentBPM() const
{
    return m_currentBpm;
}

void BpmStream::reset()
{
    m_bpmDetector.reset();
    m_currentBpm = 0.0;
    m_offset = 0;
    m_chunkCounter = 0;
}

bool BpmStream::onGetData(Chunk& data)
{
    constexpr int CHUNK_SIZE = 8192;

    if (m_offset >= m_totalSamples)
        return false;

    size_t remaining = std::min<size_t>(CHUNK_SIZE, m_totalSamples - m_offset);

    data.samples = m_samples + m_offset;
    data.sampleCount = remaining;

    m_offset += remaining;
    return true;
}

void BpmStream::onSeek(sf::Time timeOffset)
{
    m_offset = static_cast<size_t>(timeOffset.asSeconds() * m_buffer.getSampleRate() * m_buffer.getChannelCount());
}