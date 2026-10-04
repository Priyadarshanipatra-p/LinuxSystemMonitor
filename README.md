 # Linux System Resource Monitor

A real-time, terminal-based Linux system monitoring tool written in **C++17 and ncurses**, paired with a custom **C Linux kernel module** for low-level resource monitoring through `/proc`.

---

## Overview

This project provides a lightweight terminal dashboard for monitoring CPU, memory, disk, network, and running processes in real time.

The project combines:

* A **C++17 user-space monitoring application**
* The Linux **`/proc` filesystem**
* An interactive **ncurses terminal interface**
* A custom **Linux kernel module written in C**

The kernel module creates:

```text
/proc/resource_monitor
```

The user-space application reads this proc entry to display kernel-level resource information.

---

## Key Features

### CPU Monitoring

* Overall CPU utilization
* Per-core CPU utilization
* Configurable CPU warning threshold
* Real-time CPU monitoring

### Memory Monitoring

* Real-time RAM usage
* Configurable memory warning threshold
* Kernel-level total and free RAM information

### Disk Monitoring

* Filesystem disk usage
* Disk read speed
* Disk write speed
* Configurable disk warning threshold

### Network Monitoring

* Download speed
* Upload speed
* Network interface selection
* Received and transmitted packets
* Interface-level packet errors and drops
* Default interface: `eth0`

### Process Monitoring

The process table displays:

* PID
* Parent PID (PPID)
* Username
* Process name
* Process state
* Thread count
* CPU usage
* Memory usage
* Command line

Processes can be sorted by CPU or memory usage.

### Custom Linux Kernel Module

The custom kernel module is written in C and provides:

* Total RAM
* Free RAM
* CPU usage
* Total processes
* Running processes
* Sleeping processes

The module creates:

```text
/proc/resource_monitor
```

### Dynamic Alerts

The terminal interface displays:

```text
[OK]
```

when a resource is below its configured threshold and:

```text
[HIGH]
```

when the configured threshold is reached.

Configurable thresholds are available for:

* CPU
* Memory
* Disk

### Configurable Refresh Interval

The refresh interval can be configured from the command line.

Example:

```bash
./monitor --interval 1
```

The default refresh interval is `0.5` seconds.

---

## Architecture

```text
                    Linux System Resource Monitor
                              |
             +----------------+----------------+
             |                |                |
             v                v                v
      System Monitor    Process Manager   Network Monitor
             |                |                |
             v                v                v
        /proc/stat       /proc/<PID>/      /proc/net/dev
        /proc/meminfo    status/stat/
                         cmdline
             |
             v
      Custom Kernel Module
             |
             v
   /proc/resource_monitor
             |
             v
       ncurses UI
```

---

## Project Structure

```text
LinuxSystemMonitor/
│
├── include/
│   ├── SystemMonitor.h
│   ├── Process.h
│   ├── ProcessManager.h
│   ├── DiskMonitor.h
│   ├── NetworkMonitor.h
│   └── UI.h
│
├── src/
│   ├── main.cpp
│   ├── SystemMonitor.cpp
│   ├── Process.cpp
│   ├── ProcessManager.cpp
│   ├── DiskMonitor.cpp
│   ├── NetworkMonitor.cpp
│   └── UI.cpp
│
├── driver/
│   ├── resource_monitor.c
│   ├── driver_test.cpp
│   └── Makefile
│
├── Makefile
├── README.md
└── .gitignore
```

---

## Technologies Used

* **C++17**
* **C**
* **Linux**
* **Linux `/proc` filesystem**
* **Linux Kernel Module**
* **ncurses**
* **GNU Make**
* **G++**
* **WSL2**

---

# Getting Started

## Prerequisites

You need:

* Linux or WSL2
* C++17-compatible compiler
* GNU Make
* ncurses development library
* Linux kernel source/headers
* `sudo` privileges for loading the kernel module

On Debian/Ubuntu/WSL2:

```bash
sudo apt update
sudo apt install g++ make libncurses-dev build-essential
```

---

# 1. Build the Monitoring Application

Clone or enter the project:

```bash
cd ~/LinuxSystemMonitor
```

Build the application:

```bash
make
```

This creates:

```text
./monitor
```

---

# 2. Build the Kernel Module

Go to the driver directory:

```bash
cd driver
```

Build the module:

```bash
make
```

A successful build creates:

```text
resource_monitor.ko
```

---

# 3. Load the Kernel Module

Insert the kernel module:

```bash
sudo insmod resource_monitor.ko
```

Check whether it is loaded:

```bash
lsmod | grep resource_monitor
```

Read the kernel information:

```bash
cat /proc/resource_monitor
```

Example output:

```text
Linux System Monitor Kernel Module
=================================
Total RAM: 7823500 KB
Free RAM: 6683700 KB
CPU Usage: 0%
Total Processes: 216
Running Processes: 1
Sleeping Processes: 100
```

---

# 4. Run the Monitor

Return to the project directory:

```bash
cd ~/LinuxSystemMonitor
```

Run with default settings:

```bash
./monitor
```

The default configuration is:

```text
Network interface : eth0
Refresh interval  : 0.5 seconds
CPU threshold     : 80%
Memory threshold  : 80%
Disk threshold    : 80%
```

---

# Command-Line Usage

Display help:

```bash
./monitor --help
```

## Available Options

| Option                         | Description                  | Default |
| ------------------------------ | ---------------------------- | ------: |
| `--interface <name>`           | Network interface to monitor |  `eth0` |
| `--interval <seconds>`         | Refresh interval             |   `0.5` |
| `--cpu-threshold <percent>`    | CPU warning threshold        |    `80` |
| `--memory-threshold <percent>` | Memory warning threshold     |    `80` |
| `--disk-threshold <percent>`   | Disk warning threshold       |    `80` |
| `-h`, `--help`                 | Display help                 |       — |

---

## Example Commands

### Change Network Interface

```bash
./monitor --interface wlan0
```

### Change Refresh Interval

```bash
./monitor --interval 1
```

### Configure Warning Thresholds

```bash
./monitor \
    --cpu-threshold 70 \
    --memory-threshold 75 \
    --disk-threshold 80
```

### Combine Options

```bash
./monitor \
    --interface eth0 \
    --interval 1 \
    --cpu-threshold 70 \
    --memory-threshold 75 \
    --disk-threshold 80
```

---

# Technical Details

## CPU Monitoring

CPU usage is calculated using CPU time information from:

```text
/proc/stat
```

The application calculates overall CPU utilization and maintains previous CPU readings to calculate changes over time.

Per-core CPU statistics are also obtained from `/proc/stat`.

---

## Memory Monitoring

Memory information is obtained from:

```text
/proc/meminfo
```

The application calculates the current memory utilization.

The kernel module also provides total and free RAM through:

```text
/proc/resource_monitor
```

---

## Process Monitoring

Process information is collected from the Linux `/proc` filesystem.

### Process Status

```text
/proc/<PID>/status
```

Used for information including:

* Process name
* User ID
* Memory usage
* Thread count

### Process State and PPID

```text
/proc/<PID>/stat
```

Used for:

* Process state
* Parent PID
* CPU time

### Command Line

```text
/proc/<PID>/cmdline
```

Used to obtain the process command line.

---

## Disk Monitoring

Disk statistics are obtained from:

```text
/proc/diskstats
```

The application calculates:

* Read speed
* Write speed

Filesystem utilization is obtained using Linux filesystem statistics.

---

## Network Monitoring

Network statistics are obtained from:

```text
/proc/net/dev
```

The application monitors:

* Received bytes
* Transmitted bytes
* Received packets
* Transmitted packets
* Receive errors
* Transmit errors
* Receive drops
* Transmit drops

These values are used to calculate:

* Download speed
* Upload speed
* Interface-level packet errors/drops

> **Note:** The packet metric represents interface-level errors/drops, not end-to-end Internet packet loss.

---

# Linux Kernel Module

The custom kernel module is located in:

```text
driver/resource_monitor.c
```

It creates:

```text
/proc/resource_monitor
```

The module provides:

```text
Total RAM
Free RAM
CPU Usage
Total Processes
Running Processes
Sleeping Processes
```

### Kernel Module Workflow

```text
resource_monitor.c
       |
       | make
       v
resource_monitor.ko
       |
       | sudo insmod
       v
Linux Kernel
       |
       v
/proc/resource_monitor
       |
       | read
       v
C++ SystemMonitor
       |
       v
ncurses Interface
```

---

# Unloading the Kernel Module

To remove the kernel module:

```bash
sudo rmmod resource_monitor
```

After unloading, the proc entry is removed:

```text
/proc/resource_monitor
```

---

# Interactive Terminal Interface

The application uses **ncurses** to provide a real-time terminal dashboard.

The interface displays:

```text
CPU Usage
Per-Core CPU Usage
Memory Usage
Disk Usage
Disk Read Speed
Disk Write Speed
Network Download Speed
Network Upload Speed
Packet Errors/Drops
Kernel Information
Process Information
```

Resource status is displayed using:

```text
[OK]
```

or:

```text
[HIGH]
```

---

# Interactive Controls

The monitor supports keyboard controls:

| Key | Action                                 |
| --- | -------------------------------------- |
| `Q` | Quit the monitor                       |
| `S` | Change process sorting                 |
| `R` | Refresh/update the process information |

The available controls are also shown in the terminal interface.

---

# Error Handling

The application validates command-line arguments.

For example:

```bash
./monitor --interval abc
```

produces:

```text
Error: invalid refresh interval: abc
```

Invalid CPU thresholds are also rejected:

```bash
./monitor --cpu-threshold 150
```

Output:

```text
Error: CPU threshold must be between 0 and 100.
```

The application also validates:

* Missing option values
* Invalid numeric values
* Invalid refresh intervals
* Thresholds outside `0–100`
* Unknown command-line options

---

# Testing

The project has been tested for:

* C++ application compilation
* Kernel module compilation
* Kernel module loading
* `/proc/resource_monitor` creation
* Kernel resource reading
* Overall CPU monitoring
* Per-core CPU monitoring
* Memory monitoring
* Disk usage monitoring
* Disk read/write monitoring
* Network monitoring
* Network interface selection
* Packet errors/drops
* Process monitoring
* Process CPU usage
* Process memory usage
* Process sorting
* Process state
* Username
* Parent PID
* Thread count
* Command line
* Configurable refresh interval
* Configurable warning thresholds
* Command-line validation
* ncurses user interface

---

# WSL2 Environment

The project can be developed and executed in WSL2.

Check the current Linux kernel:

```bash
uname -r
```

The custom kernel module must be compiled against a compatible kernel source tree.

The development environment used for this project includes:

```text
WSL2
Linux kernel 6.18.x
C++17
ncurses
GNU Make
```

---

# Limitations

* CPU temperature monitoring is not included because WSL2 does not expose a reliable physical CPU temperature sensor to the Linux environment.
* Network packet monitoring represents interface-level packet errors/drops rather than end-to-end Internet packet loss.
* Disk speed measurements depend on the Linux disk statistics available through `/proc/diskstats`.
* Loading the kernel module requires appropriate privileges.
* Some process information may be unavailable for processes that cannot be accessed through `/proc`.
* Kernel-module behavior can depend on the running Linux/WSL2 kernel configuration.

---

# Future Scope

Possible future improvements include:

* GPU monitoring
* Detailed disk I/O statistics
* CPU temperature monitoring on native Linux hardware
* Process tree visualization
* Historical resource graphs
* Resource usage logging
* Alert notifications
* Configuration file support
* Remote system monitoring
* Additional kernel-level monitoring
* Exporting monitoring data for analysis

---

# Conclusion

The **Linux System Resource Monitor** provides a lightweight and interactive way to monitor important Linux system resources.

By combining **C++17**, the Linux `/proc` filesystem, **ncurses**, and a custom **Linux kernel module written in C**, the project demonstrates practical concepts in:

* Linux system programming
* Operating systems
* Process management
* Kernel programming
* Resource monitoring
* File-system interfaces
* C/C++ programming

The project provides real-time monitoring while demonstrating how a user-space application can communicate with and obtain information from the Linux kernel.

