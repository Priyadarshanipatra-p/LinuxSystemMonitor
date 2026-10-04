#include "NetworkMonitor.h"

#include <fstream>
#include <sstream>
#include <string>

static unsigned long long getNetworkBytes(
    unsigned long long& received,
    unsigned long long& transmitted) {

    std::ifstream file("/proc/net/dev");

    if (!file.is_open()) {
        return 0;
    }

    received = 0;
    transmitted = 0;

    std::string line;

    while (std::getline(file, line)) {

        if (line.find(':') == std::string::npos) {
            continue;
        }

        std::size_t colon =
            line.find(':');

        std::string interfaceName =
            line.substr(0, colon);

        // Remove spaces from interface name
        interfaceName.erase(
            0,
            interfaceName.find_first_not_of(" \t")
        );

        // Ignore loopback interface
        if (interfaceName == "lo") {
            continue;
        }

        std::string data =
            line.substr(colon + 1);

        std::istringstream ss(data);

        unsigned long long rxBytes = 0;
        unsigned long long txBytes = 0;

        ss >> rxBytes;

        // Skip fields between RX bytes and TX bytes
        unsigned long long value;

        for (int i = 0; i < 7; i++) {
            ss >> value;
        }

        ss >> txBytes;

        received += rxBytes;
        transmitted += txBytes;
    }

    return received + transmitted;
}

double NetworkMonitor::getDownloadSpeed() {

    unsigned long long received1;
    unsigned long long transmitted1;

    getNetworkBytes(
        received1,
        transmitted1
    );

    // This function currently returns
    // the total received data in MB.

    return static_cast<double>(received1)
           / (1024.0 * 1024.0);
}

double NetworkMonitor::getUploadSpeed() {

    unsigned long long received;
    unsigned long long transmitted;

    getNetworkBytes(
        received,
        transmitted
    );

    // This function currently returns
    // the total transmitted data in MB.

    return static_cast<double>(transmitted)
           / (1024.0 * 1024.0);
}
