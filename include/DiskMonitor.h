#ifndef DISK_MONITOR_H
#define DISK_MONITOR_H

class DiskMonitor {
private:
    unsigned long long previousReadSectors;
    unsigned long long previousWriteSectors;

    bool firstReadReading;
    bool firstWriteReading;

public:
    DiskMonitor();

    double getDiskUsage();

    double getReadSpeed();
    double getWriteSpeed();
};

#endif
