<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" style="height: 100px; "/>
</p>

# <p align="center"> WAVEWATCH IV<sup> TM</sup> (WW4<sup> TM</sup>) Compilation & Tools Documentation </p>

This directory contains utility tools for WAVEWATCH IV (WW4) developers.

## Building WAVEWATCH IV

WAVEWATCH IV uses a modern, standard CMake build system. All compilation is self-contained within the repository clone. The build system does **not** edit your shell environment (such as `.bashrc`, `.zshrc`, or system `PATH`).

Environment parameters (such as compilers, compiler flags, and search paths) are specified externally by the user when invoking CMake.

---

## Setting Up the Compile Environment

### 1. Linux Setup

On Linux operating systems (e.g., Ubuntu, RHEL, Rocky Linux):

#### Prerequisites
- **CMake** (v3.25 or higher)
- **C++ Compiler** supporting C++20 (GCC 11+, Clang 13+, or Intel `icpx`)
- **Build Utilities** (`make` or `ninja`)

#### Environment Variables
Set the standard environment variables in your active shell or pass them directly to CMake:

```bash
# Using GCC
export CXX=g++
export CXXFLAGS="-O3"

# Or using Clang / LLVM
export CXX=clang++
export CXXFLAGS="-O3"
```

#### Configuring and Building
```bash
# Development build with testing enabled
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DWW4_ENABLE_TESTING=ON
cmake --build build -j$(nproc)

# Run test suite
ctest --test-dir build --output-on-failure
```

---

### 2. macOS Setup

On macOS operating systems (Apple Silicon or Intel):

#### Prerequisites
- **Xcode Command Line Tools** (`xcode-select --install`) or Homebrew
- **CMake** (`brew install cmake`)
- **C++ Compiler** supporting C++20 (`clang++` via Apple Xcode or Homebrew `g++` / `llvm`)

#### Environment Variables
Specify the desired compiler for CMake:

```bash
# Using Apple Clang (default Xcode)
export CXX=clang++

# Or using Homebrew GCC/LLVM
export CXX=$(brew --prefix)/opt/llvm/bin/clang++
```

#### Configuring and Building
```bash
# Configure and build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DWW4_ENABLE_TESTING=ON
cmake --build build -j$(sysctl -n hw.ncpu)

# Run test suite
ctest --test-dir build --output-on-failure
```

---

## CMake Configuration Options

The following CMake options are available to customize the build:

| Option | Default | Description |
| :--- | :--- | :--- |
| `WW4_ENABLE_TESTING` | `ON` | Enable building unit and integration tests. Set to `OFF` for NOAA operations. |
| `WW4_STRICT_WARNINGS` | `OFF` | Treat compiler warnings as errors (`-Werror` or `/WX`). |
| `WW4_USE_SANITIZERS` | `OFF` | Enable Address and Undefined Behavior Sanitizers (`-fsanitize=address,undefined`). |
| `CMAKE_BUILD_TYPE` | `Release` | Build mode (`Release`, `Debug`, `RelWithDebInfo`). |

---

## Compiling for NOAA Operational Environments

In operational deployment environments, testing components (such as GoogleTest and test executables) can be completely disabled to eliminate external test dependencies and minimize build time:

```bash
cmake -B build_ops -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DWW4_ENABLE_TESTING=OFF

cmake --build build_ops
```

When `WW4_ENABLE_TESTING=OFF`:
- GoogleTest is not downloaded or required.
- Unit and integration test targets are omitted from compilation.
- Only the core libraries (`ww4_utils`, `ww4_core`) and executable (`ww4_standalone`) are built.

---

## Developer Microtools

### Test Availability Check (`ww4_test_check`)

To check if unit test coverage is present for a specific source file and its routines:

```bash
./tools/ww4_test_check --file <filename>
```
*Note: Specify the file name without extension (e.g., `time_management`).*

This tool:
1. Locates source and header files in `src/` and `include/`.
2. Extracts function/method declarations.
3. Searches `tests/` for matching unit tests.
4. Reports coverage status for each identified routine.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" style="height: 50px; width:55px;"/>
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" style="height: 50px; width:50px;"/>
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" style="height: 50px; width:75px;"/>
 </p>
