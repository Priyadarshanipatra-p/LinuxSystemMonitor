#include <vector>

#include "SystemMonitor.h"
#include "ProcessManager.h"
#include "DiskMonitor.h"
#include "NetworkMonitor.h"
#include "UI.h"

int main() {

    SystemMonitor system;

    DiskMonitor diskMonitor;

    NetworkMonitor networkMonitor;

    ProcessManager manager;

    std::vector<Process> processes =
        manager.getProcesses();

    manager.sortByCPU(processes);

    UI ui;

    ui.start(
        system,
        diskMonitor,
        networkMonitor,
        processes
    );

    return 0;
}
