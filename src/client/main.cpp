#include <iostream>
#include "filesMonitor.h" 
#include "dataFetch.h"
#include "../utilities/programKeeper.h"


int main(int argc, char* argv[]) 
{
    // The programKeeper class is used to manage the program's lifecycle
    ProgramKeeper::waitForUserInput(); // Wait for user input to exit
    
    // Create the file monitor instance

    filesMonitor fileMonitor("/home/yedidia/github/filesServer");
    dataFetch dataFetch;

    fileMonitor.attach(&dataFetch); // Attach the observer

    if (!fileMonitor.Start()) 
    {
        std::cerr << "Failed to start file monitoring." << std::endl;
        return 1;
    }

    return 0;
  
}