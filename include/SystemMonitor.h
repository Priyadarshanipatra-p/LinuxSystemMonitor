#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <string>

class SystemMonitor {
private:
    long long previousIdle;
    long long previousTotal;
    bool firstReading;

public:
    SystemMonitor();

    double getCPUUsage();
    double getMemoryUsage();

    std::string getKernelResourceInfo();
};

#endif
