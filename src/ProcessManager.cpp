#include "ProcessManager.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <pwd.h>
#include <sstream>
#include <string>
#include <unordered_map>

namespace fs = std::filesystem;

struct ProcessData {
    int pid;
    int parentPID;
    std::string username;
    std::string name;
    std::string state;
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

static std::string getUsernameFromUID(
    unsigned int uid) {

    struct passwd* passwordEntry =
        getpwuid(uid);

    if (passwordEntry != nullptr) {
        return passwordEntry->pw_name;
    }

    return "Unknown";
}

static std::string convertProcessState(
    const std::string& stateCode) {

    if (stateCode == "R") {
        return "Running";
    }

    if (stateCode == "S") {
        return "Sleeping";
    }

    if (stateCode == "D") {
        return "Waiting";
    }

    if (stateCode == "T" ||
        stateCode == "t") {
        return "Stopped";
    }

    if (stateCode == "Z") {
        return "Zombie";
    }

    if (stateCode == "X" ||
        stateCode == "x") {
        return "Dead";
    }

    if (stateCode == "I") {
        return "Idle";
    }

    return "Unknown";
}

static ProcessData readProcess(int pid) {

    ProcessData data;

    data.pid = pid;
    data.parentPID = 0;
    data.username = "Unknown";
    data.name = "Unknown";
    data.state = "Unknown";
    data.memoryKB = 0;
    data.cpuTime = 0;

    std::string statusPath =
        "/proc/" + std::to_string(pid) + "/status";

    std::ifstream statusFile(statusPath);

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

                data.name =
                    data.name.substr(first);
            }
        }

        if (line.rfind("Uid:", 0) == 0) {

            std::istringstream iss(line);

            std::string key;
            unsigned int uid = 0;

            iss >> key >> uid;

            data.username =
                getUsernameFromUID(uid);
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

            long long userTime = 0;
            long long systemTime = 0;

            /*
             * After the process name:
             *
             * Field 3  = state
             * Field 4  = parent PID
             * Field 14 = utime
             * Field 15 = stime
             *
             * Because we start after field 2:
             * state is item 1,
             * parent PID is item 2,
             * utime is item 12,
             * stime is item 13.
             */

            for (int i = 1; i <= 13; i++) {

                if (!(iss >> field)) {
                    break;
                }

                if (i == 1) {

                    data.state =
                        convertProcessState(field);
                }

                if (i == 2) {

                    try {
                        data.parentPID =
                            std::stoi(field);
                    }
                    catch (...) {
                        data.parentPID = 0;
                    }
                }

                if (i == 12) {

                    try {
                        userTime =
                            std::stoll(field);
                    }
                    catch (...) {
                        userTime = 0;
                    }
                }

                if (i == 13) {

                    try {
                        systemTime =
                            std::stoll(field);
                    }
                    catch (...) {
                        systemTime = 0;
                    }
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

    static std::unordered_map<int, long long>
        previousProcessTimes;

    static long long previousTotalCPU = 0;

    long long currentTotalCPU =
        getTotalCPUTime();

    if (currentTotalCPU == 0) {
        return processes;
    }

    long long totalCPUDifference =
        currentTotalCPU - previousTotalCPU;

    bool firstReading =
        previousTotalCPU == 0;

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

            pid =
                std::stoi(directoryName);
        }
        catch (...) {

            continue;
        }

        ProcessData data =
            readProcess(pid);

        double cpuUsage = 0.0;

        if (!firstReading &&
            totalCPUDifference > 0) {

            auto previous =
                previousProcessTimes.find(pid);

            if (previous !=
                previousProcessTimes.end()) {

                long long processDifference =
                    data.cpuTime -
                    previous->second;

                if (processDifference >= 0) {

                    cpuUsage =
                        (
                            static_cast<double>(
                                processDifference
                            )
                            /
                            static_cast<double>(
                                totalCPUDifference
                            )
                        )
                        * 100.0;
                }
            }
        }

        if (cpuUsage < 0.0) {
            cpuUsage = 0.0;
        }

        if (cpuUsage > 100.0) {
            cpuUsage = 100.0;
        }

        double memoryMB =
            static_cast<double>(
                data.memoryKB
            ) / 1024.0;

        processes.emplace_back(
            data.pid,
            data.parentPID,
            data.username,
            data.name,
            data.state,
            memoryMB,
            cpuUsage
        );

        previousProcessTimes[pid] =
            data.cpuTime;
    }

    previousTotalCPU =
        currentTotalCPU;

    return processes;
}

void ProcessManager::sortByCPU(
    std::vector<Process>& processes) {

    std::sort(
        processes.begin(),
        processes.end(),
        [](const Process& a,
           const Process& b) {

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
        [](const Process& a,
           const Process& b) {

            return a.getMemoryUsage()
                   > b.getMemoryUsage();
        }
    );
}
