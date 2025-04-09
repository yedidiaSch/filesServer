#include <iostream>
#include "filesMonitor.h" 
#include "sftpMngr.h"
#include "../utilities/programKeeper.h"


int main(int argc, char* argv[]) 
{
    
    // Create the file monitor instance

    filesMonitor fileMonitor("/home/yedidia/github/filesServer");
    
    // Create the SFTP manager instance
    sftpMngr sftp("127.0.0.1", "yedidia", "aQuila12#d", 22, "/home/yedidia/sftp");

    fileMonitor.attach(&sftp); 

    if (!fileMonitor.Start()) 
    {
        std::cerr << "Failed to start file monitoring." << std::endl;
        return 1;
    }

    // The programKeeper class is used to manage the program's lifecycle
    ProgramKeeper::waitForUserInput(); // Wait for user input to exit

    return 0;
  
}