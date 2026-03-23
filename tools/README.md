<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" style="height: 100px; "/>
</p>

# <p align="center"> WW4 tools directory </p>

Copies of tools created for  WW4 are gathered here in the tools/ directory. These are the ‘microtools` to work the repository.  ‘Macrotools’ to help develop WW4 applications, for instance to manipulate grids, are gathered in their own repositories.

The tools gathered here are documented in the [WW4 Tools](https://github.com/NOAA-EMC/WW4/wiki/Tools.md)  page of the WW4 wiki page.

# Usage

## Interactive Usage

WAVEWATCH IV provides interactive tools to help you set up and build the project.

### 1. Setup

Run the setup tool to configure your active clone and compiler settings:
```bash
./ww4_setup
```
This tool will:
- Identify and set the active WAVEWATCH IV clone in `~/.ww4_config.yml`.
- Detect available C++ compilers on your system.
- Configure compilation flags for either development or maximum optimization in `ww4_compile_config.yml`.

### 2. Compilation

Once configured, you can compile WAVEWATCH IV using:
```bash
./ww4_compile
```
This tool uses the settings from `ww4_compile_config.yml` and invokes CMake to build the project.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" style="height: 50px; width:55px;"/>
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" style="height: 50px; width:50px;"/>
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" style="height: 50px; width:75px;"/>
 </p>
