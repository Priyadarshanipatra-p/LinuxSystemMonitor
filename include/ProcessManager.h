#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <vector>
#include "Process.h"
class ProcessManager {
public:
    std::vector<Process> getProcesses();

    void sortByCPU(std::vector<Process>& processes);
    void sortByMemory(std::vector<Process>& processes);
};

#endif
