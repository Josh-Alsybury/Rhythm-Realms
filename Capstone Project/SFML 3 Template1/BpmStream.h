#ifndef BPMSTREAM_H
#define BPMSTREAM_H

#include <SFML/Audio.hpp>
#include "BPM.h" // include of bpm stream of music
#include <vector>
#include <string>


class BpmStream : public sf::SoundStream
{
public:
    BpmStream();

    bool load(const std::string& filename);
    void analyzeBPM();
    double getCurrentBPM() const;

protected:
    virtual bool onGetData(Chunk& data) override;
    virtual void onSeek(sf::Time timeOffset) override;

private:
    sf::SoundBuffer m_buffer;
    const int16_t* m_samples = nullptr;
    size_t m_totalSamples = 0;
    size_t m_offset = 0;
    mybpm::MiniBPM m_bpmDetector{ 44100.0f };
    double m_currentBpm = 0.0;

    std::vector<float> m_floatBuffer; // pre allocation 
    int m_chunkCounter = 0;
};

#endif