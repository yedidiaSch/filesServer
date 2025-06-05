#include <iostream>
#include "filesMonitor.h" 
#include "restApiMngr.h"


int main(int argc, char* argv[]) 
{
    
    // Create the file monitor instance

    filesMonitor fileMonitor("/home/yedidia/github/filesServer");
    
    // Create the REST API manager instance
    RestApiMngr apiManager("http://localhost:3000");

    fileMonitor.attach(&apiManager);

    if (!fileMonitor.Start()) 
    {
        std::cerr << "Failed to start file monitoring." << std::endl;
        return 1;
    }

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return 0;
  
}
