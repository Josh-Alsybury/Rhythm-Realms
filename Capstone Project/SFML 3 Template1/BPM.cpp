#include "BPM.h"

BPM::BPM(float smoothing)
    : source(nullptr), tempo(nullptr), buffer(nullptr),
    bpm(0.f), smoothedBPM(0.f), smoothingFactor(smoothing)
{
}

BPM::~BPM()
{
    if (buffer) del_fvec(buffer);
    if (tempo) del_aubio_tempo(tempo);
    if (source) del_aubio_source(source);
    aubio_cleanup();
}

bool BPM::loadFile(const std::string& filePath)
{
    uint_t samplerate = 0;
    uint_t hop_size = 512;

    source = new_aubio_source(filePath.c_str(), 0, hop_size);
    if (!source) {
        std::cerr << " Failed to load  file : " << filePath << std::endl;
        return false;
    }

    samplerate = aubio_source_get_samplerate(source);
    tempo = new_aubio_tempo("default", hop_size * 2, hop_size, samplerate);
    buffer = new_fvec(hop_size);

    std::cout << "Loade file: " << filePath << "  Samplerte: " << samplerate << std::endl;
    return true;
}

void BPM::update()
{
    if (!source || !tempo || !buffer)
        return;

    uint_t read = 0;
    fvec_t* out = new_fvec(2);

    aubio_source_do(source, buffer, &read);
    aubio_tempo_do(tempo, buffer, out);

    float newBPM = aubio_tempo_get_bpm(tempo);
    if (newBPM > 0)
        smoothBPM(newBPM);

    del_fvec(out);
}


void BPM::smoothBPM(float newBPM)
{
    smoothedBPM = (smoothingFactor * smoothedBPM) + ((1.0f - smoothingFactor) * newBPM);
    bpm = smoothedBPM;
}

float BPM::getBPM() const
{
    return bpm;
}

std::string BPM::getBPMState() const
{
    if (bpm < 90.f) return "Low";
    if (bpm < 120.f) return "Medium";
    return "High";
}

float BPM::getIntensityMultiplier() const
{
    if (bpm < 90.f) return 0.75f;
    if (bpm < 120.f) return 1.0f;
    return 1.25f;
}
