# Linux System Monitor

A terminal-based Linux system monitoring tool developed using **C++17**, **Linux System Programming**, **Linux Kernel Modules**, **C**, and **ncurses**.

The application monitors CPU, memory, disk, network activity, and running processes. It also integrates a custom Linux kernel module that exposes kernel-level resource information through the `/proc` filesystem.

---

## Features

### User-Space Monitoring

- Real-time CPU usage monitoring
- Real-time memory usage monitoring
- Disk usage monitoring
- Network download speed monitoring
- Network upload speed monitoring
- Running process detection
- Process CPU usage
- Process memory usage
- Sort processes by CPU usage
- Sort processes by memory usage
- CPU usage warning threshold
- Memory usage warning threshold
- Interactive ncurses terminal interface
- Manual process refresh
- Keyboard-based controls

### Kernel-Space Monitoring

A custom Linux kernel module named `resource_monitor` is included in the project.

The kernel module provides:

- Total RAM
- Free RAM
- CPU usage
- Total processes
- Running processes
- Sleeping processes

The information is exposed through:

```text
/proc/resource_monitor
