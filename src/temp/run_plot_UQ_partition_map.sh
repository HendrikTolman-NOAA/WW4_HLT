#!/usr/bin/env bash
#+ -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- +
#| WAVEWATCH IV, open source, code management by NOAA / NWS                            |
#+ -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- +
#
# Shell script wrapper to run the Python partitioning map visualization tool.
#
# Usage:
#   ./src/temp/run_plot_UQ_partition_map.sh --file partition_map.nc [--output partition_plot.png]
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
python3 "${SCRIPT_DIR}/plot_UQ_partition_map.py" "$@"
