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
    long long user;
    long long nice;
    long long system;
    long long idle;
    long long iowait;
    long long irq;
    long long softirq;
    long long steal;

    ss >> cpu
       >> user
       >> nice
       >> system
       >> idle
       >> iowait
       >> irq
       >> softirq
       >> steal;

    long long idleTime =
        idle + iowait;

    long long totalTime =
        user +
        nice +
        system +
        idle +
        iowait +
        irq +
        softirq +
        steal;

    if (firstReading) {

        previousIdle = idleTime;
        previousTotal = totalTime;

        firstReading = false;

        return 0.0;
    }

    long long idleDelta =
        idleTime - previousIdle;

    long long totalDelta =
        totalTime - previousTotal;

    previousIdle = idleTime;
    previousTotal = totalTime;

    if (totalDelta <= 0) {
        return 0.0;
    }

    double cpuUsage =
        100.0 *
        (1.0 -
         static_cast<double>(idleDelta) /
         static_cast<double>(totalDelta));

    return cpuUsage;
}

double SystemMonitor::getMemoryUsage() {

    std::ifstream file("/proc/meminfo");

    if (!file.is_open()) {
        return 0.0;
    }

    long long totalMemory = 0;
    long long availableMemory = 0;

    std::string line;

    while (std::getline(file, line)) {

        std::istringstream ss(line);

        std::string key;
        long long value;
        std::string unit;

        ss >> key >> value >> unit;

        if (key == "MemTotal:") {

            totalMemory = value;
        }
        else if (key == "MemAvailable:") {

            availableMemory = value;
        }
    }

    if (totalMemory <= 0) {
        return 0.0;
    }

    long long usedMemory =
        totalMemory - availableMemory;

    double memoryUsage =
        (static_cast<double>(usedMemory) /
         static_cast<double>(totalMemory)) *
        100.0;

    return memoryUsage;
}

std::string SystemMonitor::getKernelResourceInfo() {

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
