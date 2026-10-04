#ifndef NETWORK_MONITOR_H
#define NETWORK_MONITOR_H

#include <chrono>
#include <string>

class NetworkMonitor {
private:
    unsigned long long previousReceived;
    unsigned long long previousTransmitted;

    std::chrono::steady_clock::time_point previousTime;

    double downloadSpeed;
    double uploadSpeed;

    bool firstReading;

    std::string interfaceName;

    bool getNetworkBytes(
        unsigned long long& received,
        unsigned long long& transmitted
    );

    void updateNetworkSpeed();

public:
    NetworkMonitor(
        const std::string& interfaceName = "eth0"
    );

    double getDownloadSpeed();
    double getUploadSpeed();

    std::string getInterfaceName() const;

    bool setInterface(
        const std::string& newInterface
    );
};

#endif
