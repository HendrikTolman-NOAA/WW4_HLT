<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" height="100">
</p>

# External Libraries Used in WW4

Copies of external libraries used by WAVEWATCH IV (WW4) reside in this `./externals` directory to support fully offline development and compilation without requiring network access during `cmake build`.

## Libraries Included

- **`yaml-cpp`** (v0.8.0): C++ YAML parser library used by `ww4_utils` for reading run configuration files.
- **`googletest`** (v1.14.0): Testing framework used for unit (`L1_test_...`) and integration (`L2_test_...`) test targets when `WW4_ENABLE_TESTING` is enabled (`ON`).

## Usage

CMake automatically checks for local source directories in `./externals` (e.g. `./externals/yaml-cpp` and `./externals/googletest`). When present, CMake uses these local copies directly via `FetchContent` without performing any remote HTTP/HTTPS downloads.

If `WW4_ENABLE_TESTING` is set to `OFF` (e.g., for NOAA operational deployments), GoogleTest is skipped entirely.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" height="50" width="55">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" height="50" width="50">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" height="50" width="75">
 </p>
