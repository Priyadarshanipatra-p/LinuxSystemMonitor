#include "SystemMonitor.h"

#include <fstream>
#include <sstream>
#include <string>

SystemMonitor::SystemMonitor()
    : previousIdle(0),
      previousTotal(0),
      firstReading(true) {
}

double SystemMonitor::getCPUUsage() {

    std::ifstream file("/proc/stat");

    if (!file.is_open()) {
        return 0.0;
    }

    std::string line;
    std::getline(file, line);

    std::istringstream ss(line);

    std::string cpu;

    long long user = 0;
    long long nice = 0;
    long long system = 0;
    long long idle = 0;
    long long iowait = 0;
    long long irq = 0;
    long long softirq = 0;
    long long steal = 0;

    ss >> cpu
       >> user
       >> nice
       >> system
       >> idle
       >> iowait
       >> irq
       >> softirq
       >> steal;

    long long currentIdle =
        idle + iowait;

    long long currentTotal =
        user +
        nice +
        system +
        idle +
        iowait +
        irq +
        softirq +
        steal;

    // First reading is used as the baseline.
    if (firstReading) {

        previousIdle = currentIdle;
        previousTotal = currentTotal;

        firstReading = false;

        return 0.0;
    }

    long long idleDifference =
        currentIdle - previousIdle;

    long long totalDifference =
        currentTotal - previousTotal;

    previousIdle = currentIdle;
    previousTotal = currentTotal;

    if (totalDifference <= 0) {
        return 0.0;
    }

    double cpuUsage =
        100.0 *
        (1.0 -
         static_cast<double>(idleDifference) /
         static_cast<double>(totalDifference));

    // Keep the result between 0 and 100.
    if (cpuUsage < 0.0) {
        cpuUsage = 0.0;
    }

    if (cpuUsage > 100.0) {
        cpuUsage = 100.0;
    }

    return cpuUsage;
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

    double memoryUsage =
        100.0 *
        (1.0 -
         static_cast<double>(availableMemory) /
         static_cast<double>(totalMemory));

    if (memoryUsage < 0.0) {
        memoryUsage = 0.0;
    }

    if (memoryUsage > 100.0) {
        memoryUsage = 100.0;
    }

    return memoryUsage;
}

std::string SystemMonitor::getKernelMemoryInfo() {

    std::ifstream file("/proc/resource_monitor");

    if (!file.is_open()) {
        return "Kernel driver unavailable";
    }

    std::ostringstream output;
    std::string line;

    while (std::getline(file, line)) {
        output << line << '\n';
    }

    return output.str();
}
