#include "NetworkMonitor.h"

#include <fstream>
#include <sstream>
#include <string>

static bool getNetworkBytes(
    unsigned long long& received,
    unsigned long long& transmitted) {

    std::ifstream file("/proc/net/dev");

    if (!file.is_open()) {
        return false;
    }

    received = 0;
    transmitted = 0;

    std::string line;

    while (std::getline(file, line)) {

        if (line.find(':') == std::string::npos) {
            continue;
        }

        std::size_t colon = line.find(':');

        std::string interfaceName =
            line.substr(0, colon);

        std::size_t first =
            interfaceName.find_first_not_of(" \t");

        if (first != std::string::npos) {
            interfaceName =
                interfaceName.substr(first);
        }

        if (interfaceName == "lo") {
            continue;
        }

        std::string data =
            line.substr(colon + 1);

        std::istringstream ss(data);

        unsigned long long rxBytes = 0;
        unsigned long long txBytes = 0;
        unsigned long long value = 0;

        ss >> rxBytes;

        for (int i = 0; i < 7; i++) {
            ss >> value;
        }

        ss >> txBytes;

        received += rxBytes;
        transmitted += txBytes;
    }

    return true;
}

NetworkMonitor::NetworkMonitor()
    : previousReceived(0),
      previousTransmitted(0),
      previousTime(std::chrono::steady_clock::now()),
      downloadSpeed(0.0),
      uploadSpeed(0.0),
      firstReading(true) {
}

void NetworkMonitor::updateNetworkSpeed() {

    unsigned long long received = 0;
    unsigned long long transmitted = 0;

    if (!getNetworkBytes(received, transmitted)) {
        downloadSpeed = 0.0;
        uploadSpeed = 0.0;
        return;
    }

    auto currentTime =
        std::chrono::steady_clock::now();

    if (firstReading) {

        previousReceived = received;
        previousTransmitted = transmitted;
        previousTime = currentTime;

        firstReading = false;

        downloadSpeed = 0.0;
        uploadSpeed = 0.0;

        return;
    }

    double elapsed =
        std::chrono::duration<double>(
            currentTime - previousTime
        ).count();

    if (elapsed <= 0.0) {
        return;
    }

    unsigned long long receivedDifference =
        received - previousReceived;

    unsigned long long transmittedDifference =
        transmitted - previousTransmitted;

    downloadSpeed =
        (static_cast<double>(receivedDifference) /
         elapsed) /
        (1024.0 * 1024.0);

    uploadSpeed =
        (static_cast<double>(transmittedDifference) /
         elapsed) /
        (1024.0 * 1024.0);

    previousReceived = received;
    previousTransmitted = transmitted;
    previousTime = currentTime;
}

double NetworkMonitor::getDownloadSpeed() {

    updateNetworkSpeed();

    return downloadSpeed;
}

double NetworkMonitor::getUploadSpeed() {

    return uploadSpeed;
}
