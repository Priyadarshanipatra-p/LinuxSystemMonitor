#include "Process.h"

Process::Process(
    int pid,
    int parentPID,
    int threadCount,
    const std::string& username,
    const std::string& name,
    const std::string& state,
    const std::string& commandLine,
    double memoryUsage,
    double cpuUsage
)
    : pid(pid),
      parentPID(parentPID),
      threadCount(threadCount),
      username(username),
      name(name),
      state(state),
      commandLine(commandLine),
      memoryUsage(memoryUsage),
      cpuUsage(cpuUsage) {
}

int Process::getPID() const {
    return pid;
}

int Process::getParentPID() const {
    return parentPID;
}

int Process::getThreadCount() const {
    return threadCount;
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

std::string Process::getCommandLine() const {
    return commandLine;
}

double Process::getMemoryUsage() const {
    return memoryUsage;
}

double Process::getCPUUsage() const {
    return cpuUsage;
}
