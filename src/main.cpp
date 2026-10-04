#include <iostream>
#include <string>
#include <vector>

#include "SystemMonitor.h"
#include "ProcessManager.h"
#include "DiskMonitor.h"
#include "NetworkMonitor.h"
#include "UI.h"

int main(int argc, char* argv[]) {

    std::string interfaceName = "eth0";

    /*
     * Supported:
     *
     * ./monitor
     * ./monitor --interface eth0
     */
    for (int i = 1; i < argc; i++) {

        std::string argument = argv[i];

        if (argument == "--interface") {

            if (i + 1 >= argc) {

                std::cerr
                    << "Error: --interface requires a name\n";

                return 1;
            }

            interfaceName = argv[++i];
        }
        else if (
            argument == "--help" ||
            argument == "-h"
        ) {

            std::cout
                << "Linux System Monitor\n\n"
                << "Usage:\n"
                << "  ./monitor\n"
                << "  ./monitor --interface <name>\n\n"
                << "Options:\n"
                << "  --interface <name>  Network interface to monitor\n"
                << "  --help, -h          Show this help message\n";

            return 0;
        }
        else {

            std::cerr
                << "Unknown option: "
                << argument
                << "\n";

            std::cerr
                << "Use './monitor --help' for usage.\n";

            return 1;
        }
    }

    SystemMonitor system;

    DiskMonitor diskMonitor;

    NetworkMonitor networkMonitor(
        interfaceName
    );

    ProcessManager manager;

    std::vector<Process> processes =
        manager.getProcesses();

    manager.sortByCPU(processes);

    UI ui;

    ui.start(
        system,
        diskMonitor,
        networkMonitor,
        processes
    );

    return 0;
}
