#include "NetworkMonitor.h"

#include <fstream>
#include <sstream>

NetworkMonitor::NetworkMonitor(
    const std::string& interfaceName
)
    : previousReceived(0),
      previousTransmitted(0),
      previousReceivePackets(0),
      previousReceiveErrors(0),
      previousReceiveDrops(0),
      previousTransmitPackets(0),
      previousTransmitErrors(0),
      previousTransmitDrops(0),
      previousTime(std::chrono::steady_clock::now()),
      downloadSpeed(0.0),
      uploadSpeed(0.0),
      packetLoss(0.0),
      firstReading(true),
      interfaceName(interfaceName) {
}

bool NetworkMonitor::getNetworkStats(
    unsigned long long& received,
    unsigned long long& transmitted,
    unsigned long long& receivePackets,
    unsigned long long& receiveErrors,
    unsigned long long& receiveDrops,
    unsigned long long& transmitPackets,
    unsigned long long& transmitErrors,
    unsigned long long& transmitDrops
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

        /*
         * Receive:
         * bytes packets errs drop fifo frame
         * compressed multicast
         *
         * Transmit:
         * bytes packets errs drop fifo colls
         * carrier compressed
         */

        unsigned long long rxBytes = 0;
        unsigned long long rxPackets = 0;
        unsigned long long rxErrors = 0;
        unsigned long long rxDrops = 0;

        unsigned long long txBytes = 0;
        unsigned long long txPackets = 0;
        unsigned long long txErrors = 0;
        unsigned long long txDrops = 0;

        unsigned long long value = 0;

        ss >> rxBytes;
        ss >> rxPackets;
        ss >> rxErrors;
        ss >> rxDrops;

        /*
         * Skip:
         * fifo, frame, compressed, multicast
         */
        for (int i = 0; i < 4; i++) {
            ss >> value;
        }

        ss >> txBytes;
        ss >> txPackets;
        ss >> txErrors;
        ss >> txDrops;

        received = rxBytes;
        transmitted = txBytes;

        receivePackets = rxPackets;
        receiveErrors = rxErrors;
        receiveDrops = rxDrops;

        transmitPackets = txPackets;
        transmitErrors = txErrors;
        transmitDrops = txDrops;

        return true;
    }

    return false;
}

void NetworkMonitor::updateNetworkStats() {

    unsigned long long received = 0;
    unsigned long long transmitted = 0;

    unsigned long long receivePackets = 0;
    unsigned long long receiveErrors = 0;
    unsigned long long receiveDrops = 0;

    unsigned long long transmitPackets = 0;
    unsigned long long transmitErrors = 0;
    unsigned long long transmitDrops = 0;

    if (!getNetworkStats(
            received,
            transmitted,
            receivePackets,
            receiveErrors,
            receiveDrops,
            transmitPackets,
            transmitErrors,
            transmitDrops)) {

        downloadSpeed = 0.0;
        uploadSpeed = 0.0;
        packetLoss = 0.0;

        return;
    }

    auto currentTime =
        std::chrono::steady_clock::now();

    if (firstReading) {

        previousReceived = received;
        previousTransmitted = transmitted;

        previousReceivePackets =
            receivePackets;

        previousReceiveErrors =
            receiveErrors;

        previousReceiveDrops =
            receiveDrops;

        previousTransmitPackets =
            transmitPackets;

        previousTransmitErrors =
            transmitErrors;

        previousTransmitDrops =
            transmitDrops;

        previousTime = currentTime;

        firstReading = false;

        downloadSpeed = 0.0;
        uploadSpeed = 0.0;
        packetLoss = 0.0;

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

    unsigned long long packetDifference =
        (
            receivePackets -
            previousReceivePackets
        ) +
        (
            transmitPackets -
            previousTransmitPackets
        );

    unsigned long long errorDifference =
        (
            receiveErrors -
            previousReceiveErrors
        ) +
        (
            transmitErrors -
            previousTransmitErrors
        );

    unsigned long long dropDifference =
        (
            receiveDrops -
            previousReceiveDrops
        ) +
        (
            transmitDrops -
            previousTransmitDrops
        );

    unsigned long long lostPackets =
        errorDifference +
        dropDifference;

    if (packetDifference > 0) {

        packetLoss =
            (
                static_cast<double>(
                    lostPackets
                ) /
                static_cast<double>(
                    packetDifference +
                    lostPackets
                )
            ) * 100.0;
    }
    else {

        packetLoss = 0.0;
    }

    if (packetLoss < 0.0) {
        packetLoss = 0.0;
    }

    if (packetLoss > 100.0) {
        packetLoss = 100.0;
    }

    previousReceived = received;
    previousTransmitted = transmitted;

    previousReceivePackets =
        receivePackets;

    previousReceiveErrors =
        receiveErrors;

    previousReceiveDrops =
        receiveDrops;

    previousTransmitPackets =
        transmitPackets;

    previousTransmitErrors =
        transmitErrors;

    previousTransmitDrops =
        transmitDrops;

    previousTime = currentTime;
}

double NetworkMonitor::getDownloadSpeed() {

    updateNetworkStats();

    return downloadSpeed;
}

double NetworkMonitor::getUploadSpeed() {

    return uploadSpeed;
}

double NetworkMonitor::getPacketLoss() {

    return packetLoss;
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

    previousReceived = 0;
    previousTransmitted = 0;

    previousReceivePackets = 0;
    previousReceiveErrors = 0;
    previousReceiveDrops = 0;

    previousTransmitPackets = 0;
    previousTransmitErrors = 0;
    previousTransmitDrops = 0;

    previousTime =
        std::chrono::steady_clock::now();

    downloadSpeed = 0.0;
    uploadSpeed = 0.0;
    packetLoss = 0.0;

    firstReading = true;

    return true;
}
