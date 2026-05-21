"""
      +--------------------------------------------------------+
      | WAVEWATCH IV, open source, code management by NOAA/NWS |
      +--------------------------------------------------------+

@file ww4_setup.py
@brief Configuration tool for WAVEWATCH IV (WW4).
@details This tool provides the recommended way to interactively configure
         the active WW4 clone.

@copyright © 2026 National Weather Service, National Oceanic and Atmospheric
               Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
               of the National Weather Service.
@author Main Author(s): Aldgisl, Hendrik Tolman
@author Contributors: Jules (Agentic AI)
@date Initial: 2026-03-23
@date Last update : 2026-05-21
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
    Find all WW4 clones by searching for tools/ww4_setup or tools/ww4_setup.py.

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
        for tool_name in ["ww4_setup", "ww4_setup.py"]:
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
            # Look for either ww4_setup or ww4_setup.py in tools/
            cmd = [
                "find",
                str(search_path),
                "-maxdepth", "4",
                "-name", "ww4_setup*",
            ]
            result = subprocess.run(
                cmd, capture_output=True, text=True, check=False, timeout=10
            )
            for line in result.stdout.splitlines():
                path = Path(line).resolve()
                if path.name in ["ww4_setup", "ww4_setup.py"] and path.parent.name == "tools":
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


def update_shell_config(clone_path: Path) -> None:
    """
    Add the tools and exe directories of the clone to the user's shell configuration.

    Parameters
    ----------
    clone_path : Path
        The path to the active WW4 clone.
    """
    tools_dir = clone_path / "tools"
    exe_dir = clone_path / "exe"

    # Detect the current shell
    shell_path = os.environ.get("SHELL", "")
    shell_name = Path(shell_path).name

    # Mapping of shell to its common configuration file
    shell_configs = {
        "bash": ".bashrc",
        "zsh": ".zshrc",
        "ksh": ".kshrc",
        "sh": ".profile",
    }

    config_filename = shell_configs.get(shell_name, ".bashrc")
    config_path = Path.home() / config_filename

    # If the detected config doesn't exist, fallback to .bashrc or .profile
    if not config_path.exists():
        if (Path.home() / ".bashrc").exists():
            config_path = Path.home() / ".bashrc"
        elif (Path.home() / ".profile").exists():
            config_path = Path.home() / ".profile"
        else:
            print(f"Warning: Could not find a suitable shell configuration file (tried {config_filename}, .bashrc, .profile).")
            return

    with open(config_path, "r") as f:
        content = f.read()

    path_updates = []
    for d in [tools_dir, exe_dir]:
        export_line = f'export PATH="{d}:$PATH"'
        if export_line not in content:
            path_updates.append(export_line)

    if path_updates:
        with open(config_path, "a") as f:
            f.write("\n# WAVEWATCH IV paths\n")
            for line in path_updates:
                f.write(f"{line}\n")
        print(f"Updated {config_path} with WAVEWATCH IV paths.")
        print("Note: These changes will take effect in all NEW shell sessions.")
        print(f"To update your CURRENT session, please run: source ~/{config_path.name}")


def setup_active_clone() -> Path:
    """
    Identify and set the active WW4 clone in ~/.ww4_config.yaml.

    Returns
    -------
    Path
        The path to the selected active clone.
    """
    config_file = Path.home() / ".ww4_config.yaml"
    config = load_config(config_file)
    active_clone_str = config.get("active_clone")
    active_clone = Path(active_clone_str) if active_clone_str else None

    if active_clone and active_clone.exists():
        print(f"Current active clone found: {active_clone}")
        use_current = input("Do you want to use this clone? (y/n) [y]: ").lower().strip()
        if use_current == "" or use_current == "y":
            update_shell_config(active_clone)
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

    # Update ~/.ww4_config.yaml non-destructively
    resolved_clone = selected_clone.resolve()
    config["active_clone"] = str(resolved_clone)
    save_config(config_file, config)
    print(f"Updated {config_file} with active clone: {resolved_clone}")

    # Add tools and exe directories to shell PATH
    update_shell_config(resolved_clone)

    return resolved_clone


def main() -> None:
    """
    Main entry point for the WW4 setup tool.
    """
    print("      +--------------------------------------------------------+")
    print("      | WAVEWATCH IV, open source, code management by NOAA/NWS |")
    print("      +--------------------------------------------------------+")
    print("\n--- WAVEWATCH IV Setup Tool ---")

    setup_active_clone()

    print("\nSetup complete!")
    print("You can now build WAVEWATCH IV using standard CMake:")
    print("  cmake -B build")
    print("  cmake --build build")


if __name__ == "__main__":
    main()
