#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
private:
    int pid;
    int parentPID;
    int threadCount;

    std::string username;
    std::string name;
    std::string state;
    std::string commandLine;

    double memoryUsage;
    double cpuUsage;

public:
    Process(
        int pid,
        int parentPID,
        int threadCount,
        const std::string& username,
        const std::string& name,
        const std::string& state,
        const std::string& commandLine,
        double memoryUsage,
        double cpuUsage
    );

    int getPID() const;

    int getParentPID() const;

    int getThreadCount() const;

    std::string getUsername() const;

    std::string getName() const;

    std::string getState() const;

    std::string getCommandLine() const;

    double getMemoryUsage() const;

    double getCPUUsage() const;
};

#endif
