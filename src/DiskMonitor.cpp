#include "DiskMonitor.h"

#include <sys/statvfs.h>

#include <fstream>
#include <sstream>
#include <string>

DiskMonitor::DiskMonitor()
    : previousReadSectors(0),
      previousWriteSectors(0),
      firstReadReading(true),
      firstWriteReading(true) {
}

double DiskMonitor::getDiskUsage() {

    struct statvfs diskInfo;

    if (statvfs("/", &diskInfo) != 0) {
        return 0.0;
    }

    unsigned long long totalSpace =
        static_cast<unsigned long long>(diskInfo.f_blocks) *
        diskInfo.f_frsize;

    unsigned long long freeSpace =
        static_cast<unsigned long long>(diskInfo.f_bavail) *
        diskInfo.f_frsize;

    if (totalSpace == 0) {
        return 0.0;
    }

    unsigned long long usedSpace =
        totalSpace - freeSpace;

    return
        (static_cast<double>(usedSpace) /
         static_cast<double>(totalSpace)) * 100.0;
}

double DiskMonitor::getReadSpeed() {

    std::ifstream file("/proc/diskstats");

    if (!file.is_open()) {
        return 0.0;
    }

    std::string line;

    while (std::getline(file, line)) {

        std::istringstream ss(line);

        int major;
        int minor;
        std::string device;

        unsigned long long reads;
        unsigned long long mergedReads;
        unsigned long long readSectors;
        unsigned long long readTime;

        unsigned long long writes;
        unsigned long long mergedWrites;
        unsigned long long writeSectors;
        unsigned long long writeTime;

        ss >> major
           >> minor
           >> device
           >> reads
           >> mergedReads
           >> readSectors
           >> readTime
           >> writes
           >> mergedWrites
           >> writeSectors
           >> writeTime;

        if (device != "sdd") {
            continue;
        }

        if (firstReadReading) {

            previousReadSectors = readSectors;
            firstReadReading = false;

            return 0.0;
        }

        unsigned long long readDelta =
            readSectors - previousReadSectors;

        previousReadSectors = readSectors;

        /*
         * /proc/diskstats reports sectors.
         * One sector = 512 bytes.
         *
         * The ncurses monitor refreshes every
         * approximately 500 milliseconds.
         */
        double bytes =
            static_cast<double>(readDelta) * 512.0;

        double megabytes =
            bytes / (1024.0 * 1024.0);

        return megabytes / 0.5;
    }

    return 0.0;
}

double DiskMonitor::getWriteSpeed() {

    std::ifstream file("/proc/diskstats");

    if (!file.is_open()) {
        return 0.0;
    }

    std::string line;

    while (std::getline(file, line)) {

        std::istringstream ss(line);

        int major;
        int minor;
        std::string device;

        unsigned long long reads;
        unsigned long long mergedReads;
        unsigned long long readSectors;
        unsigned long long readTime;

        unsigned long long writes;
        unsigned long long mergedWrites;
        unsigned long long writeSectors;
        unsigned long long writeTime;

        ss >> major
           >> minor
           >> device
           >> reads
           >> mergedReads
           >> readSectors
           >> readTime
           >> writes
           >> mergedWrites
           >> writeSectors
           >> writeTime;

        if (device != "sdd") {
            continue;
        }

        if (firstWriteReading) {

            previousWriteSectors = writeSectors;
            firstWriteReading = false;

            return 0.0;
        }

        unsigned long long writeDelta =
            writeSectors - previousWriteSectors;

        previousWriteSectors = writeSectors;

        /*
         * /proc/diskstats reports sectors.
         * One sector = 512 bytes.
         *
         * The ncurses monitor refreshes every
         * approximately 500 milliseconds.
         */
        double bytes =
            static_cast<double>(writeDelta) * 512.0;

        double megabytes =
            bytes / (1024.0 * 1024.0);

        return megabytes / 0.5;
    }

    return 0.0;
}
