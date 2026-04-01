"""
      +--------------------------------------------------------+
      | WAVEWATCH IV, open source, code management by NOAA/NWS |
      +--------------------------------------------------------+

@file ww4_compile.py
@brief A CMake-based compile tool for WAVEWATCH IV (WW4).
@details This tool uses the ww4_compile_config.yml file to define the compiler
         and the compile options, and invokes CMake to build the project.

Usage
-----
1.  Configure the build environment using the recommended setup tool:
    python3 tools/ww4_setup.py

    Alternatively, configure manually by copying the template:
    cp templates/ww4_compile_config.yml ./ww4_compile_config.yml
    (And then edit the file to specify your compiler and options.)

2.  Run the compile tool from the repository root:
    python3 tools/ww4_compile.py

3.  Optionally specify a custom build directory or clean the previous build:
    python3 tools/ww4_compile.py --build-dir my_build --clean

4.  Compile immediately using a preset configuration from the templates directory
    (this bypasses the need for a local config file):
    python3 tools/ww4_compile.py --preset PRESET_CONFIG

Search Logic
------------
The tool searches for the configuration file in the following order:
1.  If --preset <PRESET> is provided, the tool uses the file
    templates/ww4_compile_config.<PRESET>.yml from the repository root.
2.  Otherwise, the directory from which the tool is called (current working directory)
    is searched for the --config file (default: ww4_compile_config.yml).
3.  The root directory of the repository clone is searched for the --config file.

If a relative path is provided via --config, it is checked in locations 2 and 3.
If an absolute path is provided, it is used directly.

@copyright © 2026 National Weather Service, National Oceanic and Atmospheric
               Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
               of the National Weather Service.
@author Main Author(s): Aldgisl (AI Persona), Hendrik Tolman
@author Contributors: Jules (Agentic AI)
@date Initial, 2026-03-20
@date Last Update, 2026-03-30
"""

import argparse
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Dict, Optional, Union

try:
    import yaml
except ImportError:
    print("Error: PyYAML is not installed. Please install it using 'pip install PyYAML'.")
    sys.exit(1)


def load_config(config_path: Path) -> Dict[str, Union[str, Dict[str, str]]]:
    """
    Load the compilation configuration from a YAML file.

    Parameters
    ----------
    config_path : Path
        The path to the ww4_compile_config.yml file.

    Returns
    -------
    Dict[str, Union[str, Dict[str, str]]]
        The configuration dictionary.

    Raises
    ------
    FileNotFoundError
        If the configuration file does not exist.
    yaml.YAMLError
        If the configuration file is not a valid YAML file.
    """
    if not config_path.exists():
        print(f"Error: Configuration file not found at {config_path}")
        print("Please copy the template to the root directory:")
        print(f"  cp templates/ww4_compile_config.yml {config_path.name}")
        sys.exit(1)

    with open(config_path, "r", encoding="utf-8") as f:
        try:
            config = yaml.safe_load(f)
            if not config or "compiler" not in config:
                print("Error: Invalid configuration format in ww4_compile_config.yml")
                sys.exit(1)
            return config
        except yaml.YAMLError as exc:
            print(f"Error parsing {config_path}: {exc}")
            sys.exit(1)


def run_command(command: list[str], cwd: Optional[Path] = None) -> None:
    """
    Run a shell command and handle errors.

    Parameters
    ----------
    command : list[str]
        The command to run as a list of strings.
    cwd : Optional[Path], optional
        The working directory to run the command in, by default None.

    Raises
    ------
    subprocess.CalledProcessError
        If the command returns a non-zero exit code.
    """
    print(f"Running: {' '.join(command)}")
    try:
        subprocess.run(command, cwd=cwd, check=True)
    except subprocess.CalledProcessError as exc:
        print(f"Command failed with exit code {exc.returncode}")
        sys.exit(exc.returncode)


def main() -> None:
    """
    Main entry point for the WW4 compile tool.
    """
    parser = argparse.ArgumentParser(description="WAVEWATCH IV Compile Tool")
    parser.add_argument(
        "--config",
        type=str,
        default="ww4_compile_config.yml",
        help="Path to the configuration file (default: ww4_compile_config.yml)",
    )
    parser.add_argument(
        "--build-dir",
        type=str,
        default="build",
        help="Build directory (default: build)",
    )
    parser.add_argument(
        "--clean", action="store_true", help="Clean the build directory before building"
    )
    parser.add_argument(
        "--preset",
        type=str,
        help="Name of the preset configuration in templates/ "
        "(e.g., 'test' for templates/ww4_compile_config.test.yml)",
    )

    args = parser.parse_args()

    cwd = Path.cwd()
    root_dir = Path(__file__).parent.parent.resolve()

    if args.preset:
        config_path = root_dir / "templates" / f"ww4_compile_config.{args.preset}.yml"
        if not config_path.exists():
            print(f"Error: Preset configuration file not found at {config_path}")
            sys.exit(1)
    else:
        config_path = Path(args.config)
        if not config_path.is_absolute():
            # Try current working directory first
            cwd_config = cwd / config_path
            # Try repository root directory second
            root_config = root_dir / config_path

            if cwd_config.exists():
                config_path = cwd_config
            elif root_config.exists():
                config_path = root_config
            else:
                # If neither exists, fall back to cwd_config for the error message in load_config
                config_path = cwd_config

    build_dir = Path(args.build_dir)
    if not build_dir.is_absolute():
        build_dir = root_dir / build_dir

    # Load configuration
    config = load_config(config_path)
    compiler_name = config["compiler"].get("name", "g++")
    compiler_options = config["compiler"].get("options", "")

    # Verify 'cmake' is available in the PATH
    if not shutil.which("cmake"):
        print("Error: 'cmake' not found in PATH.")
        print("Please install CMake and ensure it is in your PATH.")
        sys.exit(1)

    # Clean if requested
    if args.clean and build_dir.exists():
        print(f"Cleaning build directory: {build_dir}")
        shutil.rmtree(build_dir)

    # Configure
    configure_cmd = [
        "cmake",
        "-B",
        str(build_dir),
        "-S",
        str(root_dir),
        f"-DCMAKE_CXX_COMPILER={compiler_name}",
        f"-DCMAKE_CXX_FLAGS={compiler_options}",
    ]
    run_command(configure_cmd)

    # Build
    build_cmd = ["cmake", "--build", str(build_dir)]
    run_command(build_cmd)

    print("\nBuild complete!")


if __name__ == "__main__":
    main()
