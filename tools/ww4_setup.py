"""
      +--------------------------------------------------------+
      | WAVEWATCH IV, open source, code management by NOAA/NWS |
      +--------------------------------------------------------+

@file ww4_setup.py
@brief Configuration tool for WAVEWATCH IV (WW4).
@details This tool helps users configure their active WW4 clone and compiler
         settings for compilation.

@copyright © 2026 National Weather Service, National Oceanic and Atmospheric
               Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
               of the National Weather Service.
@author Aldgisl, Hendrik Tolman (Initial, 2026-03-24)
"""

import os
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional, Set

try:
    import yaml
except ImportError:
    print("Error: PyYAML is not installed. Please install it using 'pip install PyYAML'.")
    sys.exit(1)


def find_clones() -> List[Path]:
    """
    Find all WW4 clones by searching for tools/ww4_compile or tools/ww4_compile.py.

    Returns
    -------
    List[Path]
        A list of paths to the root directories of found WW4 clones.
    """
    print("Searching for WW4 clones... this may take a moment.")
    clones: List[Path] = []
    seen_paths: Set[Path] = set()

    # Check current directory and its parent first as common case
    cwd = Path.cwd()
    for p in [cwd, cwd.parent]:
        for tool_name in ["ww4_compile", "ww4_compile.py"]:
            target = p / "tools" / tool_name
            if target.exists():
                root = p.resolve()
                if root not in seen_paths:
                    clones.append(root)
                    seen_paths.add(root)
                break

    # Search in common locations
    search_paths = [Path.home(), Path("/app"), Path("/workspace")]

    for search_path in search_paths:
        if not search_path.exists():
            continue
        try:
            # Using find via subprocess for speed and robustness
            # Look for either ww4_compile or ww4_compile.py in tools/
            cmd = [
                "find",
                str(search_path),
                "-maxdepth", "4",
                "-name", "ww4_compile*",
            ]
            result = subprocess.run(
                cmd, capture_output=True, text=True, check=False, timeout=10
            )
            for line in result.stdout.splitlines():
                path = Path(line).resolve()
                if path.name in ["ww4_compile", "ww4_compile.py"] and path.parent.name == "tools":
                    clone_root = path.parent.parent
                    if clone_root not in seen_paths:
                        clones.append(clone_root)
                        seen_paths.add(clone_root)
        except subprocess.TimeoutExpired:
            print(f"Warning: Search in {search_path} timed out.")
        except Exception as e:
            print(f"Warning: Error searching in {search_path}: {e}")

    return clones


def load_config(path: Path) -> Dict[str, Any]:
    """
    Safely load a YAML configuration file.

    Parameters
    ----------
    path : Path
        The path to the YAML file.

    Returns
    -------
    Dict[str, Any]
        The configuration data, or an empty dict if the file doesn't exist.
    """
    if not path.exists():
        return {}
    with open(path, "r") as f:
        try:
            config = yaml.safe_load(f)
            return config if isinstance(config, dict) else {}
        except yaml.YAMLError as e:
            print(f"Warning: Error parsing {path}: {e}")
            return {}


def save_config(path: Path, data: Dict[str, Any], header: Optional[str] = None) -> None:
    """
    Safely save a YAML configuration file, preserving existing structure.

    Parameters
    ----------
    path : Path
        The path to the YAML file.
    data : Dict[str, Any]
        The configuration data to save.
    header : Optional[str], optional
        An optional header comment to add to the file.
    """
    with open(path, "w") as f:
        if header:
            f.write(header)
        yaml.dump(data, f, default_flow_style=False)


def setup_active_clone() -> Path:
    """
    Identify and set the active WW4 clone in ~/.ww4_config.yml.

    Returns
    -------
    Path
        The path to the selected active clone.
    """
    config_file = Path.home() / ".ww4_config.yml"
    config = load_config(config_file)
    active_clone_str = config.get("active_clone")
    active_clone = Path(active_clone_str) if active_clone_str else None

    if active_clone and active_clone.exists():
        print(f"Current active clone found: {active_clone}")
        use_current = input("Do you want to use this clone? (y/n) [y]: ").lower().strip()
        if use_current == "" or use_current == "y":
            return active_clone

    clones = find_clones()
    if not clones:
        print("Error: No WW4 clones found.")
        print("Please ensure you are running this from a WW4 clone or have one installed.")
        sys.exit(1)

    print("\nAvailable WW4 clones:")
    for i, clone in enumerate(clones):
        print(f"[{i}] {clone}")

    while True:
        try:
            choice = input(f"Select a clone [0-{len(clones)-1}]: ")
            idx = int(choice)
            if 0 <= idx < len(clones):
                selected_clone = clones[idx]
                break
        except (ValueError, IndexError):
            pass
        print("Invalid choice. Please try again.")

    # Update ~/.ww4_config.yml non-destructively
    config["active_clone"] = str(selected_clone.resolve())
    save_config(config_file, config)
    print(f"Updated {config_file} with active clone: {selected_clone}")

    return selected_clone


def setup_compiler(clone_path: Path) -> None:
    """
    Identify and set the compiler and flags in <clone>/ww4_compile_config.yml.

    Parameters
    ----------
    clone_path : Path
        The path to the active WW4 clone.
    """
    compile_config_file = clone_path / "ww4_compile_config.yml"
    config = load_config(compile_config_file)

    current_compiler = None
    if "compiler" in config and isinstance(config["compiler"], dict):
        current_compiler = config["compiler"].get("name")
        print(f"Found existing compiler setting: {current_compiler}")

    print(f"\nSetting up compiler for clone: {clone_path}")

    # Detect available compilers
    known_compilers = ["g++", "clang++", "icpx"]
    available = [c for c in known_compilers if shutil.which(c)]

    # Add a manual input option
    available.append("Manual input")

    print("Available compilers on this machine:")
    default_idx = 0
    for i, comp in enumerate(available):
        suffix = ""
        if comp == current_compiler:
            suffix = " (currently set)"
            default_idx = i
        print(f"[{i}] {comp}{suffix}")

    while True:
        try:
            prompt = f"Select a compiler [0-{len(available)-1}] [{default_idx}]: "
            choice = input(prompt).strip()
            if choice == "":
                selected_compiler = available[default_idx]
            else:
                idx = int(choice)
                if 0 <= idx < len(available):
                    selected_compiler = available[idx]
                else:
                    raise ValueError

            if selected_compiler == "Manual input":
                selected_compiler = input("Enter the C++ compiler name or path: ").strip()
                if not selected_compiler:
                    print("Compiler name cannot be empty.")
                    continue
            break
        except (ValueError, IndexError):
            print("Invalid choice. Please try again.")

    print("\nChoose compilation flags mode:")
    print("[0] Development (Strict warnings, sanitizers, -O0)")
    print("[1] Maximum Optimization (-O3, -Wall, -Wextra)")

    while True:
        choice = input("Select mode [0-1] [1]: ").strip()
        if choice == "" or choice == "1":
            flags = "-O3 -Wall -Wextra -std=c++20"
            break
        elif choice == "0":
            flags = "-O0 -g -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -std=c++20"
            break
        print("Invalid choice. Please try again.")

    # Update ww4_compile_config.yml non-destructively
    if "compiler" not in config or not isinstance(config["compiler"], dict):
        config["compiler"] = {}

    config["compiler"]["name"] = selected_compiler
    config["compiler"]["options"] = flags

    header = "# @file ww4_compile_config.yml\n# @brief WAVEWATCH IV compilation configuration.\n# @author Aldgisl, Hendrik Tolman\n\n"
    save_config(compile_config_file, config, header=header)

    print(
        f"Updated {compile_config_file} with compiler {selected_compiler} and flags: {flags}"
    )


def main() -> None:
    """
    Main entry point for the WW4 setup tool.
    """
    print("      +--------------------------------------------------------+")
    print("      | WAVEWATCH IV, open source, code management by NOAA/NWS |")
    print("      +--------------------------------------------------------+")
    print("\n--- WAVEWATCH IV Setup Tool ---")

    active_clone = setup_active_clone()
    setup_compiler(active_clone)

    print("\nSetup complete! You can now run 'ww4_compile' to build WAVEWATCH IV.")


if __name__ == "__main__":
    main()
