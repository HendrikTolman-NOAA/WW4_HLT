<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" height="100">
</p>

# External libraries used in WW4

Copies of external libraries used by WW4 can be placed here in the `./externals` directory to support offline development and compilation without requiring network access during `cmake build`.

The WW4 CMake build system automatically detects local copies of dependencies if present in this directory:
- `externals/yaml-cpp` (version 0.8.0) - Configuration file parsing
- `externals/googletest` (version 1.14.0) - C++ unit testing framework (when testing is enabled)

When these directories exist, CMake uses the local source trees directly instead of downloading source archives from external network locations.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" height="50" width="55">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" height="50" width="50">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" height="50" width="75">
 </p>
