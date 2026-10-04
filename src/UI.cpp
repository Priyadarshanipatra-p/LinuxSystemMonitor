#include "UI.h"
#include "ProcessManager.h"

#include <ncurses.h>
#include <algorithm>
#include <sstream>
#include <string>

void UI::start(
    SystemMonitor& system,
    DiskMonitor& diskMonitor,
    NetworkMonitor& networkMonitor,
    std::vector<Process>& processes) {

    initscr();

    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    timeout(500);

    start_color();

    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);

    bool running = true;
    bool sortCPU = true;

    ProcessManager manager;

    while (running) {

        clear();

        int height;
        int width;

        getmaxyx(stdscr, height, width);

        processes = manager.getProcesses();

        if (sortCPU) {
            manager.sortByCPU(processes);
        }
        else {
            manager.sortByMemory(processes);
        }

        double cpu =
            system.getCPUUsage();

        double memory =
            system.getMemoryUsage();

        double disk =
            diskMonitor.getDiskUsage();

        double download =
            networkMonitor.getDownloadSpeed();

        double upload =
            networkMonitor.getUploadSpeed();

        /*
         * Get information from Linux
         * kernel device driver.
         */
        std::string kernelMemory =
            system.getKernelResourceInfo();

        /*
         * Application title.
         */
        attron(COLOR_PAIR(1) | A_BOLD);

        mvprintw(
            1,
            std::max(0, (width - 30) / 2),
            "LINUX SYSTEM MONITOR"
        );

        attroff(COLOR_PAIR(1) | A_BOLD);

        /*
         * CPU and Memory status.
         */
        const char* cpuStatus =
            cpu >= 80.0 ? "[HIGH]" : "[OK]";

        const char* memoryStatus =
            memory >= 80.0 ? "[HIGH]" : "[OK]";

        /*
         * Normal CPU and memory values.
         */
        attron(COLOR_PAIR(2));

        mvprintw(
            3,
            2,
            "CPU Usage    : %.2f%%",
            cpu
        );

        mvprintw(
            4,
            2,
            "Memory Usage : %.2f%%",
            memory
        );

        mvprintw(
            5,
            2,
            "Disk Usage   : %.2f%%",
            disk
        );

        attroff(COLOR_PAIR(2));

        /*
         * Display warnings when CPU or memory
         * usage becomes high.
         */
        if (cpu >= 80.0) {

            attron(COLOR_PAIR(4) | A_BOLD);

            mvprintw(
                3,
                23,
                "%s",
                cpuStatus
            );

            attroff(COLOR_PAIR(4) | A_BOLD);
        }
        else {

            attron(COLOR_PAIR(2));

            mvprintw(
                3,
                23,
                "%s",
                cpuStatus
            );

            attroff(COLOR_PAIR(2));
        }

        if (memory >= 80.0) {

            attron(COLOR_PAIR(4) | A_BOLD);

            mvprintw(
                4,
                23,
                "%s",
                memoryStatus
            );

            attroff(COLOR_PAIR(4) | A_BOLD);
        }
        else {

            attron(COLOR_PAIR(2));

            mvprintw(
                4,
                23,
                "%s",
                memoryStatus
            );

            attroff(COLOR_PAIR(2));
        }

        /*
         * Network information.
         */
        attron(COLOR_PAIR(3));

        mvprintw(
            3,
            35,
            "Download : %.2f MB/s",
            download
        );

        mvprintw(
            4,
            35,
            "Upload   : %.2f MB/s",
            upload
        );

        mvprintw(
            5,
            35,
            "Processes: %lu",
            processes.size()
        );

        attroff(COLOR_PAIR(3));

        mvprintw(
            6,
            35,
            "Sorted by: %s",
            sortCPU ? "CPU" : "MEMORY"
        );

        /*
         * Linux Kernel Driver information.
         *
         * The kernel module provides:
         * Total RAM
         * Free RAM
         * CPU Usage
         * Total Processes
         * Running Processes
         * Sleeping Processes
         */
        attron(COLOR_PAIR(1) | A_BOLD);

        mvprintw(
            6,
            2,
            "Kernel Driver:"
        );

        attroff(COLOR_PAIR(1) | A_BOLD);

        int kernelRow = 7;

        std::istringstream kernelStream(kernelMemory);
        std::string kernelLine;

        /*
         * Display all kernel driver lines.
         *
         * The driver currently returns 8 lines:
         *
         * 1. Header
         * 2. Separator
         * 3. Total RAM
         * 4. Free RAM
         * 5. CPU Usage
         * 6. Total Processes
         * 7. Running Processes
         * 8. Sleeping Processes
         */
        while (std::getline(kernelStream, kernelLine)) {

            if (kernelRow >= 15) {
                break;
            }

            mvprintw(
                kernelRow,
                2,
                "%s",
                kernelLine.c_str()
            );

            kernelRow++;
        }

        /*
         * Process table.
         */
        attron(A_BOLD);

        mvprintw(
            16,
            2,
            "PID"
        );

        mvprintw(
            16,
            12,
            "PROCESS"
        );

        mvprintw(
            16,
            35,
            "CPU %%"
        );

        mvprintw(
            16,
            48,
            "MEMORY MB"
        );

        attroff(A_BOLD);

        if (width > 5) {

            mvhline(
                17,
                2,
                '-',
                width - 4
            );
        }

        int row = 18;

        int maxProcesses =
            height - 23;

        int count = 0;

        for (const Process& process :
             processes) {

            if (count >= maxProcesses) {
                break;
            }

            mvprintw(
                row,
                2,
                "%-8d",
                process.getPID()
            );

            mvprintw(
                row,
                12,
                "%-20.20s",
                process.getName().c_str()
            );

            mvprintw(
                row,
                35,
                "%8.2f",
                process.getCPUUsage()
            );

            mvprintw(
                row,
                48,
                "%10.2f",
                process.getMemoryUsage()
            );

            row++;
            count++;
        }

        /*
         * Keyboard controls.
         */
        attron(COLOR_PAIR(3) | A_BOLD);

        mvprintw(
            height - 4,
            2,
            "[R] Refresh"
        );

        mvprintw(
            height - 3,
            2,
            "[S] Sort CPU/Memory"
        );

        mvprintw(
            height - 2,
            2,
            "[Q] Quit"
        );

        attroff(COLOR_PAIR(3) | A_BOLD);

        refresh();

        int key = getch();

        if (key == 'q' || key == 'Q') {

            running = false;
        }
        else if (key == 'r' || key == 'R') {

            processes =
                manager.getProcesses();
        }
        else if (key == 's' || key == 'S') {

            sortCPU = !sortCPU;
        }
    }

    endwin();
}
