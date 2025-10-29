#include <vector>
#include <string>

namespace mybpm {


class MiniBPM
{
public:
    MiniBPM(float sampleRate);
    ~MiniBPM();

    void setBPMRange(double min, double max);
    void getBPMRange(double& min, double& max) const;
    void setBeatsPerBar(int bpb);
    int getBeatsPerBar() const;
    double estimateTempoOfSamples(const float* samples, int nsamples);
    void process(const float* samples, int nsamples);
    double estimateTempo();
    std::vector<double> getTempoCandidates() const;
    void reset();

    double estimateTempoFromFile(const std::string& filename);
    struct BPMCandidate { double bpm; double confidence; };
    std::vector<BPMCandidate> getTopCandidates(int N = 3);

private:
    class D;
    D *m_d;
};

}

