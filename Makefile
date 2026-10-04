CXX = g++

CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

LIBS = -lncurses

TARGET = monitor

SOURCES = \
    src/main.cpp \
    src/SystemMonitor.cpp \
    src/Process.cpp \
    src/ProcessManager.cpp \
    src/DiskMonitor.cpp \
    src/NetworkMonitor.cpp \
    src/UI.cpp

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)
