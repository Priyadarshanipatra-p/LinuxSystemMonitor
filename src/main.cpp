
#include <iostream>
#include <string>
#include <vector>

#include "SystemMonitor.h"
#include "ProcessManager.h"
#include "DiskMonitor.h"
#include "NetworkMonitor.h"
#include "UI.h"

static void printHelp() {

    std::cout
        << "Linux System Monitor\n\n"

        << "Usage:\n"
        << "  ./monitor\n"
        << "  ./monitor [options]\n\n"

        << "Options:\n"

        << "  --interface <name>\n"
        << "      Network interface to monitor.\n"
        << "      Default: eth0\n\n"

        << "  --interval <seconds>\n"
        << "      Refresh interval in seconds.\n"
        << "      Default: 0.5\n\n"

        << "  --cpu-threshold <percent>\n"
        << "      CPU warning threshold.\n"
        << "      Default: 80\n\n"

        << "  --memory-threshold <percent>\n"
        << "      Memory warning threshold.\n"
        << "      Default: 80\n\n"

        << "  --disk-threshold <percent>\n"
        << "      Disk warning threshold.\n"
        << "      Default: 80\n\n"

        << "  --help, -h\n"
        << "      Show this help message.\n";
}

static bool parseDouble(
    const std::string& value,
    double& result
) {

    try {

        size_t position = 0;

        double number =
            std::stod(value, &position);

        if (position != value.length()) {
            return false;
        }

        result = number;

        return true;
    }
    catch (...) {

        return false;
    }
}

static bool parsePositiveDouble(
    const std::string& value,
    double& result
) {

    if (!parseDouble(value, result)) {
        return false;
    }

    return result > 0.0;
}

int main(
    int argc,
    char* argv[]
) {

    std::string interfaceName = "eth0";

    double refreshInterval = 0.5;

    double cpuThreshold = 80.0;
    double memoryThreshold = 80.0;
    double diskThreshold = 80.0;

    for (int i = 1; i < argc; i++) {

        std::string argument =
            argv[i];

        if (
            argument == "--help" ||
            argument == "-h"
        ) {

            printHelp();

            return 0;
        }

        else if (
            argument == "--interface"
        ) {

            if (i + 1 >= argc) {

                std::cerr
                    << "Error: --interface "
                    << "requires a name.\n";

                return 1;
            }

            interfaceName =
                argv[++i];

            if (interfaceName.empty()) {

                std::cerr
                    << "Error: interface name "
                    << "cannot be empty.\n";

                return 1;
            }
        }

        else if (
            argument == "--interval"
        ) {

            if (i + 1 >= argc) {

                std::cerr
                    << "Error: --interval "
                    << "requires a positive number.\n";

                return 1;
            }

            std::string value =
                argv[++i];

            if (
                !parsePositiveDouble(
                    value,
                    refreshInterval
                )
            ) {

                std::cerr
                    << "Error: invalid refresh "
                    << "interval: "
                    << value
                    << "\n";

                return 1;
            }

            if (refreshInterval > 60.0) {

                std::cerr
                    << "Error: refresh interval "
                    << "must be between 0 and 60 "
                    << "seconds.\n";

                return 1;
            }
        }

        else if (
            argument == "--cpu-threshold"
        ) {

            if (i + 1 >= argc) {

                std::cerr
                    << "Error: --cpu-threshold "
                    << "requires a value.\n";

                return 1;
            }

            std::string value =
                argv[++i];

            if (
                !parseDouble(
                    value,
                    cpuThreshold
                )
            ) {

                std::cerr
                    << "Error: invalid CPU "
                    << "threshold: "
                    << value
                    << "\n";

                return 1;
            }

            if (
                cpuThreshold < 0.0 ||
                cpuThreshold > 100.0
            ) {

                std::cerr
                    << "Error: CPU threshold "
                    << "must be between 0 and "
                    << "100.\n";

                return 1;
            }
        }

        else if (
            argument == "--memory-threshold"
        ) {

            if (i + 1 >= argc) {

                std::cerr
                    << "Error: --memory-threshold "
                    << "requires a value.\n";

                return 1;
            }

            std::string value =
                argv[++i];

            if (
                !parseDouble(
                    value,
                    memoryThreshold
                )
            ) {

                std::cerr
                    << "Error: invalid memory "
                    << "threshold: "
                    << value
                    << "\n";

                return 1;
            }

            if (
                memoryThreshold < 0.0 ||
                memoryThreshold > 100.0
            ) {

                std::cerr
                    << "Error: memory threshold "
                    << "must be between 0 and "
                    << "100.\n";

                return 1;
            }
        }

        else if (
            argument == "--disk-threshold"
        ) {

            if (i + 1 >= argc) {

                std::cerr
                    << "Error: --disk-threshold "
                    << "requires a value.\n";

                return 1;
            }

            std::string value =
                argv[++i];

            if (
                !parseDouble(
                    value,
                    diskThreshold
                )
            ) {

                std::cerr
                    << "Error: invalid disk "
                    << "threshold: "
                    << value
                    << "\n";

                return 1;
            }

            if (
                diskThreshold < 0.0 ||
                diskThreshold > 100.0
            ) {

                std::cerr
                    << "Error: disk threshold "
                    << "must be between 0 and "
                    << "100.\n";

                return 1;
            }
        }

        else {

            std::cerr
                << "Error: unknown option: "
                << argument
                << "\n\n";

            std::cerr
                << "Use './monitor --help' "
                << "for usage.\n";

            return 1;
        }
    }

    int refreshIntervalMilliseconds =
        static_cast<int>(
            refreshInterval * 1000.0
        );

    SystemMonitor system;

    DiskMonitor diskMonitor;

    NetworkMonitor networkMonitor(
        interfaceName
    );

    ProcessManager manager;

    std::vector<Process> processes =
        manager.getProcesses();

    manager.sortByCPU(
        processes
    );

    UI ui;

    ui.start(
        system,
        diskMonitor,
        networkMonitor,
        processes,
        refreshIntervalMilliseconds,
        cpuThreshold,
        memoryThreshold,
        diskThreshold
    );

    return 0;
}
