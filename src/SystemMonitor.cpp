#include "SystemMonitor.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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


std::vector<double> SystemMonitor::getPerCoreCPUUsage() {

    std::ifstream file("/proc/stat");

    std::vector<double> coreUsage;

    if (!file.is_open()) {
        return coreUsage;
    }

    std::string line;

    /*
     * Read every line from /proc/stat.
     *
     * CPU lines look like:
     *
     * cpu
     * cpu0
     * cpu1
     * cpu2
     *
     * We ignore the first "cpu" line because
     * it represents the total CPU.
     */

    while (std::getline(file, line)) {

        if (line.compare(0, 3, "cpu") != 0) {
            break;
        }

        if (line.size() <= 3) {
            continue;
        }

        if (line[3] < '0' || line[3] > '9') {
            continue;
        }

        std::istringstream ss(line);

        std::string cpuName;

        long long user;
        long long nice;
        long long system;
        long long idle;
        long long iowait;
        long long irq;
        long long softirq;
        long long steal;

        ss >> cpuName
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

        /*
         * Determine the CPU core number.
         *
         * cpu0 -> index 0
         * cpu1 -> index 1
         * cpu2 -> index 2
         */

        int coreNumber =
            std::stoi(cpuName.substr(3));

        /*
         * Make sure our previous-value vectors
         * contain an entry for this CPU core.
         */

        if (coreNumber >=
            static_cast<int>(previousCoreIdle.size())) {

            previousCoreIdle.resize(
                coreNumber + 1,
                0
            );

            previousCoreTotal.resize(
                coreNumber + 1,
                0
            );
        }

        /*
         * First reading:
         * Store the counters and return 0%.
         */

        if (previousCoreTotal[coreNumber] == 0) {

            previousCoreIdle[coreNumber] =
                idleTime;

            previousCoreTotal[coreNumber] =
                totalTime;

            coreUsage.push_back(0.0);

            continue;
        }

        long long idleDelta =
            idleTime -
            previousCoreIdle[coreNumber];

        long long totalDelta =
            totalTime -
            previousCoreTotal[coreNumber];

        previousCoreIdle[coreNumber] =
            idleTime;

        previousCoreTotal[coreNumber] =
            totalTime;

        if (totalDelta <= 0) {

            coreUsage.push_back(0.0);

            continue;
        }

        double usage =
            100.0 *
            (1.0 -
             static_cast<double>(idleDelta) /
             static_cast<double>(totalDelta));

        /*
         * Protect against small calculation
         * inaccuracies.
         */

        if (usage < 0.0) {
            usage = 0.0;
        }

        if (usage > 100.0) {
            usage = 100.0;
        }

        coreUsage.push_back(usage);
    }

    return coreUsage;
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
