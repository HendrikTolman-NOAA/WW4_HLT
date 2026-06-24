<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" height="100">
</p>

# WAVEWATCH IV (WW4) Tests

WAVEWATCH IV uses a four-level software testing strategy, starting with the smallest components and expanding to the entire system:

- **Level 1: Unit Testing**: Focuses on the smallest testable pieces of code, typically individual functions, in isolation. These are identified by the `L1_test_` prefix and are located in `tests_L1_L2/ww4_utils/`.
- **Level 2: Integration Testing**: Individual units are combined into modules to test how they interact and uncover faults in the interfaces between components. These are identified by the `L2_test_` prefix and are located in `tests_L1_L2/ww4_core/`.
- **Level 3: Functional Testing**: Treats the software as a "black box" and focuses on testing the application against expected results for specific physics cases.
- **Level 4: Regression Testing**: Validation process focused on the full model application to ensure that updates haven't negatively impacted established behaviors.

This directory currently contains the unit tests (Level 1) and integration tests (Level 2) for the WAVEWATCH IV project.

## Running Tests Interactively

Interactive tests can be run from the **project root directory** or directly from the **build directory**.

### From the Project Root

To configure, build, and run tests from the main directory:

1.  **Configure the build:**
    ```bash
    cmake -B build -S .
    ```
2.  **Build the tests:**
    ```bash
    cmake --build build
    ```
3.  **Run the tests:**
    ```bash
    ctest --test-dir build
    ```

### From the Build Directory

Alternatively, if you are already in the `build/` directory:

1.  **Run the tests:**
    ```bash
    ctest
    ```

## Running Tests Automatically

These tests are integrated into the project's Continuous Integration (CI) pipeline. They are executed automatically:

- On every push to any branch.
- For every pull request submitted to the repository.

The CI environment ensures that all tests pass before any changes can be merged, maintaining the stability and reliability of the codebase.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" height="50" ; width="55">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" height="50" width="50">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" height="50" width="75">
 </p>
