<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" height="100">
</p>

# External libraries used in WW4

External libraries used by WW4 are included as Git submodules in the `./externals` directory:
- `externals/yaml-cpp` (version 0.8.0) - Configuration file parsing
- `externals/netcdf-c` (NetCDF-C) - NetCDF / UGRID file reading (NetCDF-4 is enabled when HDF5 and ZLIB are found, otherwise NetCDF-3 classic only)
- `externals/googletest` (version 1.14.0) - C++ unit testing framework (when testing is enabled)

## Installed libraries versus submodules

By default (`-DWW4_DEPENDENCY_MODE=AUTO`), CMake first looks for installed copies of `yaml-cpp`, `NetCDF-C` and `GoogleTest`, for example from environment modules. Any library it does not find is built from the matching submodule here.

| `WW4_DEPENDENCY_MODE` | Behavior |
|---|---|
| `AUTO` (default) | Use installed libraries when found, otherwise the submodule. |
| `SYSTEM` | Use installed libraries only. Configuration fails if one is missing. Recommended for operations. |
| `SUBMODULE` | Always build the submodules and ignore installed libraries. |

For module-provided libraries, CMake must be able to locate them. Either the module sets `CMAKE_PREFIX_PATH`, or you pass a root such as `-DnetCDF_ROOT=...` or `-Dyaml-cpp_ROOT=...`, or the library's `bin/` directory is on `PATH`.

If a needed submodule is empty, CMake runs `git submodule update --init` for it (disable with `-DWW4_AUTO_INIT_SUBMODULES=OFF`).

## Cloning with submodules

To clone WW4 with external dependencies included, use:
```bash
git clone --recursive https://github.com/NOAA-EMC/WW4.git
```

If the repository was cloned without `--recursive`, initialize and update submodules using:
```bash
git submodule update --init --recursive
```

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" height="50" width="55">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" height="50" width="50">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" height="50" width="75">
 </p>
