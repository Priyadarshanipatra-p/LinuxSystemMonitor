#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::ifstream file("/proc/resource_monitor");

    if (!file)
    {
        std::cerr << "Error: Could not open /proc/resource_monitor\n";
        return 1;
    }

    std::string line;

    while (std::getline(file, line))
    {
        std::cout << line << '\n';
    }

    file.close();

    return 0;
}
