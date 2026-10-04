#ifndef SYSTEM_MONITOR_H
#define SYSTEM_MONITOR_H

#include <string>
#include <vector>

class SystemMonitor {
private:
    long long previousIdle;
    long long previousTotal;
    bool firstReading;

    std::vector<long long> previousCoreIdle;
    std::vector<long long> previousCoreTotal;

public:
    SystemMonitor();

    double getCPUUsage();
    double getMemoryUsage();

    std::vector<double> getPerCoreCPUUsage();

    std::string getKernelResourceInfo();
};

#endif
