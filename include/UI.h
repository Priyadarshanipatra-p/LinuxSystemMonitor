#ifndef UI_H
#define UI_H

#include <vector>

#include "Process.h"
#include "SystemMonitor.h"
#include "DiskMonitor.h"
#include "NetworkMonitor.h"

class UI {
public:

    void start(
        SystemMonitor& system,
        DiskMonitor& diskMonitor,
        NetworkMonitor& networkMonitor,
        std::vector<Process>& processes
    );
};

#endif
