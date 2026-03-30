"""
      +--------------------------------------------------------+
      | WAVEWATCH IV, open source, code management by NOAA/NWS |
      +--------------------------------------------------------+

@file ww4_clean_all.py
@brief A tool to remove all compilation output, including executables and libraries.
@details This tool removes the 'build' directory and all files in 'exe' (except README.md).

@copyright © 2026 National Weather Service, National Oceanic and Atmospheric
               Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
               of the National Weather Service.
@author Aldgisl, Hendrik Tolman (Initial, 2026-03-27)
@date 2026-03-27
"""

import os
import shutil
import sys
from pathlib import Path

def main() -> None:
    """
    Main entry point for the WW4 Clean All tool.
    """
    # Repository root is the parent of the directory containing this script
    root_dir = Path(__file__).parent.parent.resolve()
    build_dir = root_dir / "build"
    exe_dir = root_dir / "exe"

    # Remove build directory
    if build_dir.exists() and build_dir.is_dir():
        print(f"Removing build directory: {build_dir}")
        try:
            shutil.rmtree(build_dir)
            print("Build directory removed.")
        except Exception as e:
            print(f"Error removing build directory: {e}")
            sys.exit(1)
    else:
        print("No build directory found.")

    # Remove contents of exe directory (except README.md)
    if exe_dir.exists() and exe_dir.is_dir():
        print(f"Cleaning executables in: {exe_dir}")
        for item in os.listdir(exe_dir):
            if item == "README.md":
                continue
            item_path = exe_dir / item
            try:
                if item_path.is_dir():
                    shutil.rmtree(item_path)
                else:
                    item_path.unlink()
            except Exception as e:
                print(f"Error removing {item_path}: {e}")
        print("Executable directory cleaned.")
    else:
        print("No executable directory found.")

    print("\nClean all complete.")

if __name__ == "__main__":
    main()
