<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" height="100">
</p>

# <p align="center"> WAVEWATCH IV<sup> TM</sup> (WW4<sup> TM</sup>) Configuration Templates </p>

This directory contains template configuration files for running WAVEWATCH IV simulations.

## Runtime Configuration Templates

1. **`ww4_run_config.yaml`**: Configures model dynamics, physics, forcing inputs, output options, and model time-step settings.
2. **`ww4_standalone.yaml`**: Configures stand-alone simulation metadata, including simulation start and end times.

## Usage

To use these configuration templates for a simulation run:

1. Copy the template files to the root directory of the clone or working execution directory:
    ```bash
    cp templates/ww4_run_config.yaml ./ww4_run_config.yaml
    cp templates/ww4_standalone.yaml ./ww4_standalone.yaml
    ```
2. Edit `ww4_run_config.yaml` and `ww4_standalone.yaml` with your simulation parameters.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" height="50" ; width="55">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" height="50" width="50">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" height="50" width="75">
 </p>
