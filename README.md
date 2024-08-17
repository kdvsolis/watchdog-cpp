# Watchdog Project

## Overview

The Watchdog project is designed to manage and monitor processes on Windows. It includes functionalities for starting, stopping, and checking the status of processes, as well as bringing them to the foreground and retrieving the name of the currently active foreground process.

## Features

- **Process Management**: Start, stop, and monitor processes.
- **Process Foreground Management**: Bring processes to the foreground.
- **Foreground Process Retrieval**: Get the name of the currently active foreground process.

## Requirements

- **Windows OS**: The project is tailored for Windows environments.
- **C++ Standard**: C++17.
- **Visual Studio 2022**: For building and development.
- **vcpkg**: For managing dependencies (e.g., GTest for unit testing).

## Dependencies

- **nlohmann/json**: For JSON handling.
- **GoogleTest (GTest)**: For unit testing.

## Building the Project

1. **Clone the Repository**

   ```bash
   git clone https://github.com/your-repository/watchdog.git
   cd watchdog
   ```

2. **Install Dependencies**

   Make sure you have `vcpkg` installed and configured. Install the required dependencies using:

   ```bash
   vcpkg install nlohmann-json gtest
   ```

3. **Generate Build Files with CMake**

   Open a command prompt and navigate to the project's root directory. Run:

   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE="/path/to/vcpkg-2024.07.12/scripts/buildsystems/vcpkg.cmake" -DGTest_DIR="/path/to/vcpkg-2024.07.12/installed/x64-windows/share/gtest
   ```

4. **Build the Project**

   ```bash
   cmake --build . --config Debug
   ```

## Running Tests

1. **Build the Test Executable**

   Ensure you have followed the steps to build the project and tests as described above.

2. **Run Tests**

   After building, run the test executable to perform unit tests:

   ```bash
   ./build/Debug/test_NativeAPI.exe
   ```

   The test results will be displayed in the console.

## Project Structure

- `src/`: Contains source files for the main project.
- `include/`: Contains header files.
- `tests/`: Contains unit tests for the project.
- `config/`: Contains configuration files.
- `CMakeLists.txt`: CMake build configuration file.
- `README.md`: This file.

## Notes

- The `Watchdog` application is specifically designed for Windows and uses Windows API calls for process management.
- The provided tests are mock implementations and may not interact with real processes.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.
