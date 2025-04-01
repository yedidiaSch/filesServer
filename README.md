# filesServer

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

A file monitoring application that automatically sends content over the network to a server for remote execution.

## 📋 Features

- **Real-time file monitoring** - Detects file creation, modification, deletion, and attribute changes
- **Customizable filters** - Configure which files to monitor with pattern matching


## 🔧 Requirements

- Linux-based system 
- C++11 compatible compiler
- cmake (for building)
- Google Test (for running tests)

## 🚀 Building the Project

```bash
# Clone the repository
git clone https://github.com/yourusername/filesServer.git
cd filesServer

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make
```

## 📖 Usage

### Basic Usage

```cpp
#include "filesMonitor/filesMonitor.h"
#include <iostream>

// Create a file observer implementation
class FileObserver : public IObserver {
public:
    void update(void* params) override {
        std::string* filename = static_cast<std::string*>(params);
        std::cout << "File event detected: " << *filename << std::endl;
        delete filename; // Don't forget to free the memory
    }
};

int main() {
    // Create a file monitor for a specific directory
    filesMonitor monitor("/path/to/watch");
    
    // Create and attach observer
    FileObserver observer;
    monitor.attach(&observer);
    
    // Add filters if needed (optional)
    monitor.AddFilter(".cpp");  // Only watch C++ files
    
    // Start monitoring
    if (monitor.Start()) {
        std::cout << "Monitoring started successfully" << std::endl;
        
        // Keep program running
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.get();
        
        // Stop monitoring
        monitor.Stop();
    }
    
    return 0;
}
```

## 🧪 Testing

The project includes comprehensive test suites using Google Test:

```bash
# Run tests from the build directory
./test_utilities
```



## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.


## 📜 License

This project is licensed under the MIT License - see the LICENSE file for details.



