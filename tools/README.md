<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" style="height: 100px; "/>
</p>

# <p align="center"> WW4 tools directory </p>

Copies of tools created for WAVEWATCH IV (WW4) are gathered here in the `tools/` directory. These are the microtools to support repository management and testing. Macrotools to help develop WW4 applications, for instance to manipulate grids, are gathered in their own repositories.

The tools gathered here are documented on the [WW4 Tools](https://github.com/NOAA-EMC/WW4/wiki/Tools.md) page of the WW4 wiki.

# Compilation and Setup

WAVEWATCH IV uses a standard CMake build system. Build configuration is handled locally in each repository clone using CMake options and standard environment variables. The build system does not modify the user's interactive environment or shell profile scripts.

## Setting the Compiler

You can specify the C++ compiler either by exporting the `CXX` environment variable or by passing `-DCMAKE_CXX_COMPILER` directly to CMake.

### Using Environment Variables:
```bash
# Using GCC
export CXX=g++
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Using Clang
export CXX=clang++
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Using Intel LLVM (icpx)
export CXX=icpx
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
```

### Specifying the Compiler in CMake Command:
```bash
# Explicitly set the C++ compiler via CMake
cmake -B build -S . -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
cmake -B build -S . -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release
```

## Compiler and Build Options

WW4 supports standard CMake build configuration flags:

- **Build Type (`-DCMAKE_BUILD_TYPE`):** Set to `Release`, `Debug`, `RelWithDebInfo`, or `MinSizeRel`.
  ```bash
  cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
  ```
- **Strict Compiler Warnings (`-DWW4_STRICT_WARNINGS=ON|OFF`):** Enables strict compiler warnings (`-Wall -Wextra -Wpedantic -Werror` / `/W4 /WX`) with appropriate suppressions for external dependencies on supported compilers. Default is `OFF`.
  ```bash
  cmake -B build -S . -DWW4_STRICT_WARNINGS=ON
  ```
- **Sanitizers (`-DWW4_USE_SANITIZERS=ON|OFF`):** Enables AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan). Default is `OFF`.
  ```bash
  cmake -B build -S . -DWW4_USE_SANITIZERS=ON
  ```
- **Enable/Disable Testing (`-DWW4_ENABLE_TESTING=ON|OFF`):** Controls whether GoogleTest submodules and test targets are included in the build system. Default is `ON`.
  ```bash
  # For NOAA operational builds without testing components:
  cmake -B build_ops -S . -DWW4_ENABLE_TESTING=OFF
  cmake --build build_ops
  ```

## Building WAVEWATCH IV

```bash
# Configure build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build executables and libraries
cmake --build build
```

## Cleaning Build Artifacts

To remove compiled object files, static libraries, and executables (including unit test executables located in `build/bin/`) generated during a build:

```bash
# Clean built object files, libraries, and executables target-by-target
cmake --build build --target clean
```

To perform a complete clean and remove all generated build directories and executable output folders:

```bash
# Remove build tree directory completely
rm -rf build

# Remove standalone executable directory if present
rm -rf exe
```

# Developer Tools

WAVEWATCH IV provides additional tools to support developers during the coding process.

## Test Availability Check

To check if unit tests are available for a specific file and its identified routines:

```bash
./tools/ww4_test_check --file <filename>
```
*Note: The filename should be provided without extension (e.g., `time_management`).*

This tool will:
- Identify source files in `src/` and `include/`.
- Isolate user-defined routines (functions and methods).
- Check the `tests/` directory for corresponding unit tests.
- Report the test coverage status for each identified routine.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" style="height: 50px; width:55px;"/>
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" style="height: 50px; width:50px;"/>
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" style="height: 50px; width:75px;"/>
 </p>
