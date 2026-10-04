#include "DiskMonitor.h"

#include <sys/statvfs.h>

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
