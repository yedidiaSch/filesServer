#include <iostream>
#include "filesMonitor/filesMonitor.h" 
#include "filesMonitor/dataFetch.h"



int main(int argc, char* argv[]) 
{
    // Create the file monitor instance

    filesMonitor fileMonitor("/home/yedidia/github/filesServer");
    dataFetch dataFetch;

    fileMonitor.attach(&dataFetch); // Attach the observer

    if (!fileMonitor.Start()) 
    {
        std::cerr << "Failed to start file monitoring." << std::endl;
        return 1;
    }

    std::cout << "Monitoring started. Press Enter to stop..." << std::endl;

    std::cin.get(); // Wait for user input to stop monitoring

    fileMonitor.Stop(); // Stop monitoring

    std::cout << "Monitoring stopped." << std::endl;

    return 0;
  
}