#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <chrono>

class NetworkMonitor {
private:
    unsigned long long previousReceived;
    unsigned long long previousTransmitted;

    std::chrono::steady_clock::time_point previousTime;

    double downloadSpeed;
    double uploadSpeed;

    bool firstReading;

    void updateNetworkSpeed();

public:
    NetworkMonitor();

    double getDownloadSpeed();
    double getUploadSpeed();
};

#endif
