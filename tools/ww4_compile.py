"""
      +--------------------------------------------------------+
      | WAVEWATCH IV, open source, code management by NOAA/NWS |
      +--------------------------------------------------------+

@file ww4_compile.py
@brief A CMake-based compile tool for WAVEWATCH IV (WW4).
@details This tool uses the ww4_compile_config.yml file in the repository root
         directory to define the compiler and the compile options, and invokes
         CMake to build the project.
@copyright © 2026 National Weather Service, National Oceanic and Atmospheric
               Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
               of the National Weather Service.
@author Aldgisl (Initial, 2026-03-20)
@author Aldgisl (Last Update, 2026-03-20)
@date 2026-03-20
"""

import argparse
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

    args = parser.parse_args()

    root_dir = Path(__file__).parent.parent.resolve()
    config_path = root_dir / args.config
    build_dir = root_dir / args.build_dir

    # Load configuration
    config = load_config(config_path)
    compiler_name = config["compiler"].get("name", "g++")
    compiler_options = config["compiler"].get("options", "")

    # Clean if requested
    if args.clean and build_dir.exists():
        import shutil

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
