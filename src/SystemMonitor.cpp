#include "SystemMonitor.h"

#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>

double SystemMonitor::getCPUUsage() {
    std::ifstream file("/proc/stat");

    if (!file.is_open()) {
        return 0.0;
    }

    std::string line;
    std::getline(file, line);

    std::istringstream ss(line);

    std::string cpu;
    long long user, nice, system, idle;
    long long iowait, irq, softirq, steal;

    ss >> cpu
       >> user
       >> nice
       >> system
       >> idle
       >> iowait
       >> irq
       >> softirq
       >> steal;

    long long idleTime = idle + iowait;

    long long totalTime =
        user + nice + system + idle +
        iowait + irq + softirq + steal;

    if (totalTime == 0) {
        return 0.0;
    }

    return 100.0 *
           (1.0 - static_cast<double>(idleTime) / totalTime);
}

double SystemMonitor::getMemoryUsage() {
    std::ifstream file("/proc/meminfo");

    if (!file.is_open()) {
        return 0.0;
    }

    long long totalMemory = 0;
    long long availableMemory = 0;

    std::string key;
    long long value;
    std::string unit;

    while (file >> key >> value >> unit) {

        if (key == "MemTotal:") {
            totalMemory = value;
        }

        if (key == "MemAvailable:") {
            availableMemory = value;
        }
    }

    if (totalMemory == 0) {
        return 0.0;
    }

    return 100.0 *
           (1.0 -
            static_cast<double>(availableMemory) / totalMemory);
}

