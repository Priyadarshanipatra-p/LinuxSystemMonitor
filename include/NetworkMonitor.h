#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <chrono>
#include <string>

class NetworkMonitor {
private:
    unsigned long long previousReceived;
    unsigned long long previousTransmitted;

    unsigned long long previousReceivePackets;
    unsigned long long previousReceiveErrors;
    unsigned long long previousReceiveDrops;

    unsigned long long previousTransmitPackets;
    unsigned long long previousTransmitErrors;
    unsigned long long previousTransmitDrops;

    std::chrono::steady_clock::time_point previousTime;

    double downloadSpeed;
    double uploadSpeed;
    double packetLoss;

    bool firstReading;

    std::string interfaceName;

    bool getNetworkStats(
        unsigned long long& received,
        unsigned long long& transmitted,
        unsigned long long& receivePackets,
        unsigned long long& receiveErrors,
        unsigned long long& receiveDrops,
        unsigned long long& transmitPackets,
        unsigned long long& transmitErrors,
        unsigned long long& transmitDrops
    );

    void updateNetworkStats();

public:
    NetworkMonitor(
        const std::string& interfaceName = "eth0"
    );

    double getDownloadSpeed();
    double getUploadSpeed();
    double getPacketLoss();

    std::string getInterfaceName() const;

    bool setInterface(
        const std::string& newInterface
    );
};

#endif
