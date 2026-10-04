#include "Process.h"

Process::Process(
    int pid,
    const std::string& name,
    const std::string& state,
    double memoryUsage,
    double cpuUsage
)
    : pid(pid),
      name(name),
      state(state),
      memoryUsage(memoryUsage),
      cpuUsage(cpuUsage) {
}

int Process::getPID() const {
    return pid;
}

std::string Process::getName() const {
    return name;
}

std::string Process::getState() const {
    return state;
}

double Process::getMemoryUsage() const {
    return memoryUsage;
}

double Process::getCPUUsage() const {
    return cpuUsage;
}
