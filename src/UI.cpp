#include "UI.h"
#include "ProcessManager.h"

#include <ncurses.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

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

        std::vector<double> coreUsage =
            system.getPerCoreCPUUsage();

        double disk =
            diskMonitor.getDiskUsage();

        double diskRead =
            diskMonitor.getReadSpeed();

        double diskWrite =
            diskMonitor.getWriteSpeed();

        double download =
            networkMonitor.getDownloadSpeed();

        double upload =
            networkMonitor.getUploadSpeed();

        double packetLoss =
            networkMonitor.getPacketLoss();

        std::string networkInterface =
            networkMonitor.getInterfaceName();

        std::string kernelMemory =
            system.getKernelResourceInfo();

        attron(
            COLOR_PAIR(1) |
            A_BOLD
        );

        mvprintw(
            1,
            std::max(0, (width - 30) / 2),
            "LINUX SYSTEM MONITOR"
        );

        attroff(
            COLOR_PAIR(1) |
            A_BOLD
        );

        const char* cpuStatus =
            cpu >= 80.0 ? "[HIGH]" : "[OK]";

        const char* memoryStatus =
            memory >= 80.0 ? "[HIGH]" : "[OK]";

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

        mvprintw(
            6,
            2,
            "Disk Read    : %.2f MB/s",
            diskRead
        );

        mvprintw(
            7,
            2,
            "Disk Write   : %.2f MB/s",
            diskWrite
        );

        attroff(COLOR_PAIR(2));

        if (cpu >= 80.0) {

            attron(
                COLOR_PAIR(4) |
                A_BOLD
            );

            mvprintw(
                3,
                23,
                "%s",
                cpuStatus
            );

            attroff(
                COLOR_PAIR(4) |
                A_BOLD
            );
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

            attron(
                COLOR_PAIR(4) |
                A_BOLD
            );

            mvprintw(
                4,
                23,
                "%s",
                memoryStatus
            );

            attroff(
                COLOR_PAIR(4) |
                A_BOLD
            );
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
            "Interface: %s",
            networkInterface.c_str()
        );

        mvprintw(
            6,
            35,
            "Packet Loss: %.2f%%",
            packetLoss
        );

        mvprintw(
            7,
            35,
            "Processes: %lu",
            processes.size()
        );

        mvprintw(
            8,
            35,
            "Sorted by: %s",
            sortCPU ? "CPU" : "MEMORY"
        );

        attroff(COLOR_PAIR(3));

        attron(
            COLOR_PAIR(1) |
            A_BOLD
        );

        mvprintw(
            10,
            2,
            "CPU Cores (%lu):",
            coreUsage.size()
        );

        attroff(
            COLOR_PAIR(1) |
            A_BOLD
        );

        int coreStartRow = 11;

        for (
            size_t i = 0;
            i < coreUsage.size();
            i++
        ) {

            int column =
                static_cast<int>(i % 4);

            int row =
                coreStartRow +
                static_cast<int>(i / 4);

            int x =
                2 + column * 15;

            if (coreUsage[i] >= 80.0) {

                attron(
                    COLOR_PAIR(4) |
                    A_BOLD
                );
            }
            else {

                attron(COLOR_PAIR(2));
            }

            mvprintw(
                row,
                x,
                "Core%-2zu %6.2f%%",
                i,
                coreUsage[i]
            );

            if (coreUsage[i] >= 80.0) {

                attroff(
                    COLOR_PAIR(4) |
                    A_BOLD
                );
            }
            else {

                attroff(COLOR_PAIR(2));
            }
        }

        int kernelTitleRow = 16;

        attron(
            COLOR_PAIR(1) |
            A_BOLD
        );

        mvprintw(
            kernelTitleRow,
            2,
            "Kernel Driver:"
        );

        attroff(
            COLOR_PAIR(1) |
            A_BOLD
        );

        int kernelRow =
            kernelTitleRow + 1;

        std::istringstream kernelStream(
            kernelMemory
        );

        std::string kernelLine;

        while (
            std::getline(
                kernelStream,
                kernelLine
            )
        ) {

            if (kernelRow >= 25) {
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

        int processHeaderRow =
            std::max(
                26,
                kernelRow + 1
            );

        attron(A_BOLD);

        mvprintw(
            processHeaderRow,
            2,
            "PID"
        );

        mvprintw(
            processHeaderRow,
            11,
            "PPID"
        );

        mvprintw(
            processHeaderRow,
            20,
            "USER"
        );

        mvprintw(
            processHeaderRow,
            31,
            "PROCESS"
        );

        mvprintw(
            processHeaderRow,
            53,
            "STATE"
        );

        mvprintw(
            processHeaderRow,
            66,
            "CPU %%"
        );

        mvprintw(
            processHeaderRow,
            79,
            "MEMORY MB"
        );

        attroff(A_BOLD);

        if (
            width > 5 &&
            processHeaderRow + 1 < height
        ) {

            mvhline(
                processHeaderRow + 1,
                2,
                '-',
                width - 4
            );
        }

        int row =
            processHeaderRow + 2;

        int maxProcesses =
            height - row - 6;

        if (maxProcesses < 0) {
            maxProcesses = 0;
        }

        int count = 0;

        for (
            const Process& process :
            processes
        ) {

            if (count >= maxProcesses) {
                break;
            }

            mvprintw(
                row,
                2,
                "%-7d",
                process.getPID()
            );

            mvprintw(
                row,
                11,
                "%-7d",
                process.getParentPID()
            );

            mvprintw(
                row,
                20,
                "%-10.10s",
                process.getUsername().c_str()
            );

            mvprintw(
                row,
                31,
                "%-20.20s",
                process.getName().c_str()
            );

            mvprintw(
                row,
                53,
                "%-12.12s",
                process.getState().c_str()
            );

            mvprintw(
                row,
                66,
                "%8.2f",
                process.getCPUUsage()
            );

            mvprintw(
                row,
                79,
                "%10.2f",
                process.getMemoryUsage()
            );

            row++;
            count++;
        }

        attron(
            COLOR_PAIR(3) |
            A_BOLD
        );

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

        attroff(
            COLOR_PAIR(3) |
            A_BOLD
        );

        refresh();

        int key = getch();

        if (
            key == 'q' ||
            key == 'Q'
        ) {

            running = false;
        }
        else if (
            key == 'r' ||
            key == 'R'
        ) {

            processes =
                manager.getProcesses();
        }
        else if (
            key == 's' ||
            key == 'S'
        ) {

            sortCPU = !sortCPU;
        }
    }

    endwin();
}
