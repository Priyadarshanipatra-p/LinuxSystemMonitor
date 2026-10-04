#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
private:
    int pid;
    int parentPID;
    std::string username;
    std::string name;
    std::string state;
    double memoryUsage;
    double cpuUsage;

public:
    Process(
        int pid,
        int parentPID,
        const std::string& username,
        const std::string& name,
        const std::string& state,
        double memoryUsage,
        double cpuUsage
    );

    int getPID() const;

    int getParentPID() const;

    std::string getUsername() const;

    std::string getName() const;

    std::string getState() const;

    double getMemoryUsage() const;

    double getCPUUsage() const;
};

#endif

