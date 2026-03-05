<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" height="100">
</p>

# WAVEWATCH IV (WW4) Doxygen Documentation

This directory is the output location for the Doxygen-generated technical documentation for the WAVEWATCH IV project.

## Running Doxygen Interactively

Documentation must be generated from the **project root directory** to ensure that the Doxygen tool can correctly locate and process all source files and headers within the repository structure.

### From the Project Root

To generate the documentation:

1.  **Ensure Doxygen is installed:**
    ```bash
    doxygen --version
    ```
2.  **Run Doxygen:**
    ```bash
    doxygen Doxyfile
    ```
    *Note: If a `Doxyfile` is not present, you can generate a default one using `doxygen -g` or use the one provided in the repository.*

3.  **View the documentation:**
    Once generated, open `docs/html/index.html` in your web browser.

## Running Doxygen Automatically

Technical documentation is integrated into the project's Continuous Integration (CI) pipeline. It is executed automatically:

- On every push to the `main` or `develop` branches.
- For every pull request submitted to the repository to ensure documentation integrity.

The CI environment generates the latest documentation and may publish it to a dedicated hosting service (e.g., GitHub Pages) to ensure that the project's technical reference is always up-to-date.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" height="50" width="55">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" height="50" width="50">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" height="50" width="75">
 </p>
