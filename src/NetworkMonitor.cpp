#include "NetworkMonitor.h"

#include <fstream>
#include <sstream>

NetworkMonitor::NetworkMonitor(
    const std::string& interfaceName
)
    : previousReceived(0),
      previousTransmitted(0),
      previousTime(std::chrono::steady_clock::now()),
      downloadSpeed(0.0),
      uploadSpeed(0.0),
      firstReading(true),
      interfaceName(interfaceName) {
}

bool NetworkMonitor::getNetworkBytes(
    unsigned long long& received,
    unsigned long long& transmitted
) {

    std::ifstream file("/proc/net/dev");

    if (!file.is_open()) {
        return false;
    }

    std::string line;

    while (std::getline(file, line)) {

        std::size_t colon =
            line.find(':');

        if (colon == std::string::npos) {
            continue;
        }

        std::string currentInterface =
            line.substr(0, colon);

        std::size_t first =
            currentInterface.find_first_not_of(" \t");

        if (first == std::string::npos) {
            continue;
        }

        currentInterface =
            currentInterface.substr(first);

        if (currentInterface != interfaceName) {
            continue;
        }

        std::string data =
            line.substr(colon + 1);

        std::istringstream ss(data);

        unsigned long long rxBytes = 0;
        unsigned long long txBytes = 0;

        ss >> rxBytes;

        /*
         * Receive fields:
         * bytes packets errs drop fifo frame
         * compressed multicast
         *
         * Skip the remaining 7 fields.
         */
        unsigned long long value = 0;

        for (int i = 0; i < 7; i++) {
            ss >> value;
        }

        /*
         * The next value is transmitted bytes.
         */
        ss >> txBytes;

        received = rxBytes;
        transmitted = txBytes;

        return true;
    }

    return false;
}

void NetworkMonitor::updateNetworkSpeed() {

    unsigned long long received = 0;
    unsigned long long transmitted = 0;

    if (!getNetworkBytes(
            received,
            transmitted)) {

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
        (
            static_cast<double>(
                receivedDifference
            ) / elapsed
        ) /
        (1024.0 * 1024.0);

    uploadSpeed =
        (
            static_cast<double>(
                transmittedDifference
            ) / elapsed
        ) /
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

std::string NetworkMonitor::getInterfaceName() const {

    return interfaceName;
}

bool NetworkMonitor::setInterface(
    const std::string& newInterface
) {

    if (newInterface.empty()) {
        return false;
    }

    interfaceName = newInterface;

    /*
     * Reset measurements so that changing
     * interfaces does not produce an incorrect
     * speed calculation.
     */
    previousReceived = 0;
    previousTransmitted = 0;

    previousTime =
        std::chrono::steady_clock::now();

    downloadSpeed = 0.0;
    uploadSpeed = 0.0;

    firstReading = true;

    return true;
}
