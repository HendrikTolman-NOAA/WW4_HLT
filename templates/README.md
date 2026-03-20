<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" height="100">
</p>

# <p align="center"> WAVEWATCH IV<sup> TM</sup> (WW4<sup> TM</sup>) Configuration Templates </p>

This directory contains template files for configuring the WAVEWATCH IV build environment. These templates provide a starting point for users to set up their local compilation environment.

## Interactive usage

Users can manually copy the templates from this directory to the desired location (typically the repository root) and modify them as needed.

1.  Copy `templates/compilation.yaml` to the root directory:
    ```bash
    cp templates/compilation.yaml ./compilation.yaml
    ```
2.  Edit `compilation.yaml` to specify your compiler and preferred options.

## Automatic usage

Build scripts or CI/CD pipelines can use these templates to generate default configurations if a user-specified configuration is not found.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" height="50" ; width="55">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" height="50" width="50">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" height="50" width="75">
 </p>
