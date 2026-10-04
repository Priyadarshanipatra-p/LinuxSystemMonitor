# Linux System Monitor

A terminal-based Linux system monitoring tool developed using **C++17**, Linux system interfaces, and **ncurses**.

The project monitors CPU, memory, disk, network activity, and running processes through Linux system information and provides an interactive terminal interface.

---

## Features

- Real-time CPU usage monitoring
- Real-time memory usage monitoring
- Disk usage monitoring
- Real-time network download speed
- Real-time network upload speed
- Running process detection
- Real-time process CPU usage
- Process memory usage
- Sort processes by CPU usage
- Sort processes by memory usage
- CPU usage warning threshold
- Memory usage warning threshold
- Interactive ncurses terminal interface
- Manual process refresh
- Keyboard-based controls

---

## Technologies Used

- **C++17**
- **Linux**
- **Linux `/proc` filesystem**
- **ncurses**
- **STL**
- **C++17 filesystem**
- **statvfs()**
- **Makefile**
- **WSL2 / Ubuntu**
- **Git & GitHub**

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
├── Makefile
├── README.md
└── .gitignore
