<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" style="height: 100px; "/>
</p>

# <p align="center"> WW4 tools directory </p>

Copies of tools created for  WW4 are gathered here in the tools/ directory. These are the ‘microtools` to work the repository.  ‘Macrotools’ to help develop WW4 applications, for instance to manipulate grids, are gathered in their own repositories.

The tools gathered here are documented in the [WW4 Tools](https://github.com/NOAA-EMC/WW4/wiki/Tools.md)  page of the WW4 wiki page.

# Usage

WAVEWATCH IV provides multiple ways to set up and build the project, ranging from interactive tools to manual configuration.

## Interactive Usage (Recommended)

Run the setup tool to interactively configure your active clone and compiler settings:

```bash
./ww4_setup
```
This tool will:
- Identify and set the active WAVEWATCH IV clone in `~/.ww4_config.yml`.
- Detect available C++ compilers on your system.
- Configure compilation flags for either development or maximum optimization in `ww4_compile_config.yml`.


## Developer Tools

WAVEWATCH IV provides additional tools to support developers during the coding process.

### L1/L2 Test Availability Check

To check if unit tests are available for a specific file and its identified routines:

```bash
./ww4_L1_L2_test_check --file <filename>
```
*Note: The filename should be provided without extension (e.g., `time_management`).*

This tool will:
- Identify source files in `src/` and `include/`.
- Isolate user-defined routines (functions and methods).
- Check the `tests_L1_L2/` directory for corresponding unit tests.
- Report the test coverage status for each identified routine.

### Clean Tools

To remove intermediate files created during compilation:

```bash
./ww4_clean
```

To remove all compilation output, including executables and libraries:

```bash
./ww4_clean_all
```

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" style="height: 50px; width:55px;"/>
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" style="height: 50px; width:50px;"/>
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" style="height: 50px; width:75px;"/>
 </p>
