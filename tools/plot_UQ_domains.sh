#!/bin/bash
#      +--------------------------------------------------------+
#      | WAVEWATCH IV, open source, code management by NOAA/NWS |
#      +--------------------------------------------------------+
#
# @file plot_UQ_domains.sh
# @brief Shell wrapper for the WAVEWATCH IV UQ domain plotting tool.
# @details This script identifies the repository root and executes the
#          underlying Python plotting tool (plot_UQ_domains.py).
#
# Usage
# -----
# Run the tool from any directory within the clone:
#    plot_UQ_domains.sh <domain_file.nc> [options]
#    plot_UQ_domains.sh --input <domain_file.nc> [options]
#
# @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
#               Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
#               of the National Weather Service.
# The National Weather Service (NWS) often uses Generative SI (GenSI, formally GenAI) tools for code development and refactoring. Whenever such tools are used, NWS requires disclosure and a full human review of changes before they are applied to NWS repositories.
# @author Main Author(s): Aldgisl (AI Persona), Hendrik Tolman
# @author Contributors: Jules (Agentic AI), Kiro (Agentic AI/SI)
# @date Initial: 2026-11-04
# @date Last update: 2026-11-16

# Identify the repository root (parent of the directory containing this script)
TOOLS_DIR=$(cd "$(dirname "$0")" && pwd)
ACTIVE_CLONE=$(cd "$TOOLS_DIR/.." && pwd)

PLOT_TOOL="$ACTIVE_CLONE/tools/plot_UQ_domains.py"

if [ ! -f "$PLOT_TOOL" ]; then
    echo "Error: Plotting tool not found at $PLOT_TOOL"
    exit 1
fi

python3 "$PLOT_TOOL" "$@"
