"""
      +--------------------------------------------------------+
      | WAVEWATCH IV, open source, code management by NOAA/NWS |
      +--------------------------------------------------------+

@file ww4_Clean.py
@brief A tool to remove intermediate files created by compilation.
@details This tool removes the 'build' directory in the repository root.

@copyright © 2026 National Weather Service, National Oceanic and Atmospheric
               Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
               of the National Weather Service.
@author Aldgisl, Hendrik Tolman (Initial, 2026-03-27)
@date 2026-03-27
"""

import shutil
import sys
from pathlib import Path

def main() -> None:
    """
    Main entry point for the WW4 Clean tool.
    """
    # Repository root is the parent of the directory containing this script
    root_dir = Path(__file__).parent.parent.resolve()
    build_dir = root_dir / "build"

    if build_dir.exists() and build_dir.is_dir():
        print(f"Removing build directory: {build_dir}")
        try:
            shutil.rmtree(build_dir)
            print("Clean complete.")
        except Exception as e:
            print(f"Error removing build directory: {e}")
            sys.exit(1)
    else:
        print("No build directory found. Nothing to clean.")

if __name__ == "__main__":
    main()
