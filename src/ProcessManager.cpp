#include "ProcessManager.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unistd.h>

namespace fs = std::filesystem;

struct ProcessData {
    int pid;
    std::string name;
    long memoryKB;
    long long cpuTime;
};

static long long getTotalCPUTime() {

    std::ifstream file("/proc/stat");

    if (!file.is_open()) {
        return 0;
    }

    std::string line;
    std::getline(file, line);

    std::istringstream ss(line);

    std::string cpu;
    long long user = 0;
    long long nice = 0;
    long long system = 0;
    long long idle = 0;
    long long iowait = 0;
    long long irq = 0;
    long long softirq = 0;
    long long steal = 0;

    ss >> cpu
       >> user
       >> nice
       >> system
       >> idle
       >> iowait
       >> irq
       >> softirq
       >> steal;

    return user + nice + system + idle +
           iowait + irq + softirq + steal;
}

static ProcessData readProcess(int pid) {

    ProcessData data;

    data.pid = pid;
    data.name = "Unknown";
    data.memoryKB = 0;
    data.cpuTime = 0;

    std::string path =
        "/proc/" + std::to_string(pid) + "/status";

    std::ifstream statusFile(path);

    if (!statusFile.is_open()) {
        return data;
    }

    std::string line;

    while (std::getline(statusFile, line)) {

        if (line.rfind("Name:", 0) == 0) {

            data.name = line.substr(5);

            size_t first =
                data.name.find_first_not_of(" \t");

            if (first != std::string::npos) {
                data.name = data.name.substr(first);
            }
        }

        if (line.rfind("VmRSS:", 0) == 0) {

            std::istringstream iss(line);

            std::string key;
            std::string unit;

            iss >> key
                >> data.memoryKB
                >> unit;
        }
    }

    /*
       /proc/[PID]/stat

       Field 14 = user CPU time
       Field 15 = system CPU time
    */

    std::string statPath =
        "/proc/" + std::to_string(pid) + "/stat";

    std::ifstream statFile(statPath);

    if (statFile.is_open()) {

        std::string statLine;

        std::getline(statFile, statLine);

        size_t closeParen =
            statLine.rfind(')');

        if (closeParen != std::string::npos) {

            std::string remaining =
                statLine.substr(closeParen + 2);

            std::istringstream iss(remaining);

            std::string field;

            /*
               After removing PID and process name,
               field 3 becomes the state.

               Therefore:
               field 14 → position 12 here
               field 15 → position 13 here
            */

            long long userTime = 0;
            long long systemTime = 0;

            for (int i = 1; i <= 13; i++) {

                iss >> field;

                if (i == 12) {
                    userTime = std::stoll(field);
                }

                if (i == 13) {
                    systemTime = std::stoll(field);
                }
            }

            data.cpuTime =
                userTime + systemTime;
        }
    }

    return data;
}

std::vector<Process> ProcessManager::getProcesses() {

    std::vector<Process> processes;

    long long totalCPU =
        getTotalCPUTime();

    if (totalCPU == 0) {
        return processes;
    }

    for (const auto& entry :
         fs::directory_iterator("/proc")) {

        if (!entry.is_directory()) {
            continue;
        }

        std::string directoryName =
            entry.path().filename().string();

        if (directoryName.empty() ||
            directoryName.find_first_not_of("0123456789")
                != std::string::npos) {

            continue;
        }

        int pid;

        try {
            pid = std::stoi(directoryName);
        }
        catch (...) {
            continue;
        }

        ProcessData data =
            readProcess(pid);

        double cpuUsage =
            (static_cast<double>(data.cpuTime)
             / totalCPU) * 100.0;

        double memoryMB =
            static_cast<double>(data.memoryKB)
            / 1024.0;

        processes.emplace_back(
            data.pid,
            data.name,
            memoryMB,
            cpuUsage
        );
    }

    return processes;
}

void ProcessManager::sortByCPU(
    std::vector<Process>& processes) {

    std::sort(
        processes.begin(),
        processes.end(),
        [](const Process& a, const Process& b) {

            return a.getCPUUsage()
                   > b.getCPUUsage();
        }
    );
}

void ProcessManager::sortByMemory(
    std::vector<Process>& processes) {

    std::sort(
        processes.begin(),
        processes.end(),
        [](const Process& a, const Process& b) {

            return a.getMemoryUsage()
                   > b.getMemoryUsage();
        }
    );
}
