#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <mutex>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

class SpotifyClient
{
public:
    struct TrackInfo
    {
        std::string trackName;
        std::string artistName;
        std::string albumName;
        int durationMs;
        int progressMs;
        bool isPlaying;
    };

    SpotifyClient();
    ~SpotifyClient();

    void StartPolling();
    void StopPolling();
    TrackInfo GetCurrentTrack();

private:
    std::thread m_pollingThread;
    std::atomic<bool> m_isRunning;
    std::mutex m_trackMutex;
    TrackInfo m_currentTrack;

    void PollingLoop();
    std::string HttpGet(const std::string& host, const std::string& path);
    void ParseTrackData(const std::string& jsonResponse);
};