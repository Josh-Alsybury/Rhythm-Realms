#include "SpotifyClient.h"
#include <iostream>
#include <chrono>

SpotifyClient::SpotifyClient()
    : m_isRunning(false)
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

SpotifyClient::~SpotifyClient()
{
    StopPolling();
#ifdef _WIN32
    WSACleanup();
#endif
}

void SpotifyClient::StartPolling()
{
    if (m_isRunning)
        return;

    m_isRunning = true;
    m_pollingThread = std::thread(&SpotifyClient::PollingLoop, this);
    std::cout << "Spotify polling started" << std::endl;
}

void SpotifyClient::StopPolling()
{
    if (!m_isRunning)
        return;

    m_isRunning = false;
    if (m_pollingThread.joinable())
        m_pollingThread.join();

    std::cout << "Spotify polling stopped" << std::endl;
}

SpotifyClient::TrackInfo SpotifyClient::GetCurrentTrack()
{
    std::lock_guard<std::mutex> lock(m_trackMutex);
    return m_currentTrack;
}


void SpotifyClient::PollingLoop()
{
    while (m_isRunning)
    {
        try
        {
            std::string response = HttpGet("127.0.0.1:8888", "/current");

            if (!response.empty())
            {
                ParseTrackData(response);
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Polling error: " << e.what() << std::endl;
        }

        // Poll every 2 seconds
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

std::string SpotifyClient::HttpGet(const std::string& host, const std::string& path)
{
#ifdef _WIN32
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
        return "";

    // Parse host and port
    std::string hostName = host.substr(0, host.find(':'));
    std::string portStr = host.substr(host.find(':') + 1);
    int port = std::stoi(portStr);

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, hostName.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        closesocket(sock);
        return "";
    }

    // Send HTTP GET request
    std::string request = "GET " + path + " HTTP/1.1\r\n";
    request += "Host: " + hostName + "\r\n";
    request += "Connection: close\r\n\r\n";

    send(sock, request.c_str(), request.length(), 0);

    // Receive response
    std::string response;
    char buffer[4096];
    int bytesReceived;

    while ((bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0)
    {
        buffer[bytesReceived] = '\0';
        response += buffer;
    }

    closesocket(sock);

    // Extract JSON body (after headers)
    size_t bodyStart = response.find("\r\n\r\n");
    if (bodyStart != std::string::npos)
    {
        return response.substr(bodyStart + 4);
    }

    return "";
#else
   
    return "";
#endif
}

void SpotifyClient::ParseTrackData(const std::string& jsonResponse)
{
    std::lock_guard<std::mutex> lock(m_trackMutex);

    if (jsonResponse.find("\"error\"") != std::string::npos)
    {
        m_currentTrack.isPlaying = false;
        return;
    }

    m_currentTrack.isPlaying = (jsonResponse.find("\"is_playing\":true") != std::string::npos);

    // Extract track name
    size_t namePos = jsonResponse.find("\"name\":");
    if (namePos != std::string::npos)
    {
        size_t start = jsonResponse.find("\"", namePos + 7) + 1;
        size_t end = jsonResponse.find("\"", start);
        m_currentTrack.trackName = jsonResponse.substr(start, end - start);
    }

    std::cout << "Current track: " << m_currentTrack.trackName
        << " (Playing: " << m_currentTrack.isPlaying << ")" << std::endl;
}