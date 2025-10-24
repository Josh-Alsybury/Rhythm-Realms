#pragma once
#include <aubio/aubio.h>
#include <iostream>
class BPM
{
public:
    BPM(float smoothing = 0.9f);
    ~BPM();

    bool loadFile(const std::string& filePath);
    void update(); 
    float getBPM() const;
    std::string getBPMState() const; 
    float getIntensityMultiplier() const; 

private:

    aubio_source_t* source;
    aubio_tempo_t* tempo;
    fvec_t* buffer;

    float bpm;
    float smoothedBPM;
    float smoothingFactor;

    void smoothBPM(float newBPM);
};

