#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
private:
    int pid;
    std::string name;
    double memoryUsage;
    double cpuUsage;

public:
    Process(int pid,
            const std::string& name,
            double memoryUsage,
            double cpuUsage);

    int getPID() const;
    std::string getName() const;
    double getMemoryUsage() const;
    double getCPUUsage() const;
};

#endif
