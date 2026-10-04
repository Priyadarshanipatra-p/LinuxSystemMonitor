#ifndef UI_H
#define UI_H

#include <vector>

#include "SystemMonitor.h"
#include "ProcessManager.h"
#include "DiskMonitor.h"
#include "NetworkMonitor.h"

class UI {
public:

    void start(
        SystemMonitor& system,
        DiskMonitor& diskMonitor,
        NetworkMonitor& networkMonitor,
        std::vector<Process>& processes,
        int refreshIntervalMilliseconds,
        double cpuThreshold,
        double memoryThreshold,
        double diskThreshold
    );
};

#endif
