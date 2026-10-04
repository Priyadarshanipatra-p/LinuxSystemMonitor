#include "Process.h"

Process::Process(
    int pid,
    int parentPID,
    const std::string& username,
    const std::string& name,
    const std::string& state,
    double memoryUsage,
    double cpuUsage
)
    : pid(pid),
      parentPID(parentPID),
      username(username),
      name(name),
      state(state),
      memoryUsage(memoryUsage),
      cpuUsage(cpuUsage) {
}

int Process::getPID() const {
    return pid;
}

int Process::getParentPID() const {
    return parentPID;
}

std::string Process::getUsername() const {
    return username;
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
