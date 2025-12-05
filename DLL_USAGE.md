# Using libpololu-tic as a C++ DLL

This document explains how to build and use the Pololu Tic library as a Dynamic Link Library (DLL) on Windows or as a shared library on Linux/macOS.

## Overview

The libpololu-tic library is already configured to build as a shared library/DLL by default. The library is written in C but provides full C++ compatibility through:

1. **C API** (`tic.h`) - C functions with `extern "C"` linkage for compatibility
2. **C++ Wrapper** (`tic.hpp`) - Modern C++ classes wrapping the C API
3. **DLL Export Macros** - Proper symbol visibility for shared libraries

## Building as a DLL/Shared Library

### Default Configuration

By default, the library builds as a shared library. The CMake option `BUILD_SHARED_LIBS` is set to `TRUE`.

### Windows DLL Build

To build the library as a DLL on Windows with MinGW:

```bash
# In MSYS2 MinGW shell
mkdir build
cd build
cmake .. -G "MSYS Makefiles"
make
```

To build with Visual Studio:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
```

The output will be:
- `pololu-tic-1.dll` - The dynamic link library
- `libpololu-tic-1.dll.a` - Import library (MinGW)
- `pololu-tic-1.lib` - Import library (MSVC)

### Linux/macOS Shared Library Build

```bash
mkdir build
cd build
cmake ..
make
```

The output will be:
- `libpololu-tic-1.so` (Linux)
- `libpololu-tic-1.dylib` (macOS)

### Building as a Static Library

If you prefer a static library instead:

```bash
cmake .. -DBUILD_SHARED_LIBS=FALSE
```

This will create `libpololu-tic-1.a` (Unix) or `libpololu-tic-1.lib` (Windows).

## Using the DLL in Your C++ Project

### Including Headers

Include the appropriate header in your C++ source files:

```cpp
// For C++ wrapper (recommended)
#include <tic.hpp>

// Or for C API
#include <tic.h>
```

### Linking

#### With CMake

Create a `CMakeLists.txt` in your project:

**Method 1: Using pkg-config (recommended if library is installed to system):**

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyTicApp)

set(CMAKE_CXX_STANDARD 11)

# Find the Tic library using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(TIC REQUIRED libpololu-tic-1)

add_executable(myapp main.cpp)

target_include_directories(myapp PRIVATE ${TIC_INCLUDE_DIRS})
target_link_libraries(myapp ${TIC_LIBRARIES})
target_compile_options(myapp PRIVATE ${TIC_CFLAGS_OTHER})
```

**Method 2: Manual library finding (if library is not in standard location):**

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyTicApp)

set(CMAKE_CXX_STANDARD 11)

# Manually find the Tic library
find_path(TIC_INCLUDE_DIR tic.hpp
  PATHS /usr/local/include /usr/include
  PATH_SUFFIXES libpololu-tic-1
)

find_library(TIC_LIBRARY
  NAMES pololu-tic-1
  PATHS /usr/local/lib /usr/lib
)

add_executable(myapp main.cpp)

target_include_directories(myapp PRIVATE ${TIC_INCLUDE_DIR})
target_link_libraries(myapp ${TIC_LIBRARY})

# On Windows, you may need to add the DLL directory to PATH
if(WIN32)
  get_filename_component(TIC_DLL_DIR ${TIC_LIBRARY} DIRECTORY)
  set_target_properties(myapp PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "PATH=${TIC_DLL_DIR};$ENV{PATH}"
  )
endif()
```

**Method 3: Specifying custom installation prefix:**

If you installed the library to a custom location (e.g., `/opt/tic`):

```bash
cmake .. -DCMAKE_PREFIX_PATH=/opt/tic
```

#### Manual Linking

**MinGW/GCC:**
```bash
g++ -std=c++11 main.cpp -lpololu-tic-1 -o myapp
```

**Visual Studio:**
Add to your project:
- Include directories: Path to `include/libpololu-tic-1`
- Library directories: Path to the DLL's directory
- Additional dependencies: `pololu-tic-1.lib`

### Deployment

When distributing your application:

**Windows:**
- Include `pololu-tic-1.dll` in the same directory as your executable
- Or install it to a system PATH directory

**Linux:**
- Install the shared library to `/usr/local/lib` or another library path
- Run `ldconfig` to update the library cache
- Or set `LD_LIBRARY_PATH` to include the library directory

**macOS:**
- Install the shared library to `/usr/local/lib`
- Or set `DYLD_LIBRARY_PATH` to include the library directory

## Example C++ Program

Here's a simple example using the C++ wrapper:

```cpp
#include <tic.hpp>
#include <iostream>

int main()
{
  try
  {
    // List connected Tic devices
    std::vector<tic::device> devices = tic::list_connected_devices();
    
    if (devices.empty())
    {
      std::cerr << "No Tic devices found." << std::endl;
      return 1;
    }
    
    std::cout << "Found " << devices.size() << " Tic device(s):" << std::endl;
    
    // Display information about each device
    for (const auto & device : devices)
    {
      std::cout << "  " << device.get_name() 
                << " #" << device.get_serial_number() << std::endl;
    }
    
    // Connect to the first device
    tic::handle handle(devices[0]);
    
    // Exit safe start
    handle.exit_safe_start();
    
    // Set target position
    handle.set_target_position(1000);
    
    // Get current variables
    tic::variables vars = handle.get_variables();
    std::cout << "Current position: " << vars.get_current_position() << std::endl;
    
  }
  catch (const tic::error & e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
```

## API Documentation

The library provides two levels of API:

### C API (`tic.h`)

The C API provides functions for:
- Device enumeration (`tic_list_connected_devices`)
- Device connection (`tic_handle_open`, `tic_handle_close`)
- Motor control (`tic_set_target_position`, `tic_set_target_velocity`)
- Status retrieval (`tic_get_variables`)
- Settings management (`tic_get_settings`, `tic_set_settings`)
- Error handling (`tic_error_get_message`, `tic_error_free`)

All C functions use the prefix `tic_` and follow C naming conventions.

### C++ API (`tic.hpp`)

The C++ API provides classes that wrap the C API:
- `tic::device` - Represents a Tic device
- `tic::handle` - Connection to a Tic device
- `tic::variables` - Current status variables
- `tic::settings` - Device settings
- `tic::error` - Error handling with exceptions

The C++ wrapper uses RAII for automatic resource management, exceptions for error handling, and provides a more modern C++ interface.

## Symbol Export/Import Macros

The library uses the following macros for DLL export/import:

- `TIC_API` - Marks functions for export from DLL or import when using DLL
- `TIC_EXPORTS` - Defined when building the library (exports symbols)
- `TIC_STATIC` - Defined when building/using as a static library

These are automatically handled by CMake and you don't need to define them manually when using the library.

## Troubleshooting

### Windows: DLL not found

Make sure the DLL is in:
1. The same directory as your executable
2. A directory in your PATH environment variable
3. Or use `SetDllDirectory()` to specify the DLL location programmatically

### Linux: Shared library not found

```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
# Or
sudo ldconfig
```

### macOS: Shared library not found

```bash
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
```

### Linking errors

If you get undefined reference errors:
- Ensure you're linking against the import library (.lib or .dll.a)
- Check that the library was built for the same architecture (32-bit vs 64-bit)
- Verify that `TIC_STATIC` is defined if using the static library

## Additional Resources

- [Building from Source](BUILDING.md) - Detailed build instructions
- [Tic User's Guide](https://www.pololu.com/docs/0J71) - Official documentation
- [API Reference](include/tic.h) - C API documentation
- [C++ Wrapper](include/tic.hpp) - C++ API documentation
