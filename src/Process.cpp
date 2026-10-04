#include "Process.h"

Process::Process(int pid,
                 const std::string& name,
                 double memoryUsage,
                 double cpuUsage)
    : pid(pid),
      name(name),
      memoryUsage(memoryUsage),
      cpuUsage(cpuUsage) {
}

int Process::getPID() const {
    return pid;
}

std::string Process::getName() const {
    return name;
}

double Process::getMemoryUsage() const {
    return memoryUsage;
}

double Process::getCPUUsage() const {
    return cpuUsage;
}
