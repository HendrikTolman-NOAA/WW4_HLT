"""
      +--------------------------------------------------------+
      | WAVEWATCH IV, open source, code management by NOAA/NWS |
      +--------------------------------------------------------+

@file plot_partition_map.py
@brief Python tool to display WAVEWATCH IV grid partitioning maps from NetCDF files.
@details Reads 2D partition map arrays from .nc NetCDF files and generates a color-coded
         grid visualization with colorbar and axis labels.

Usage
-----
    python3 src/temp/plot_partition_map.py --file partition_map.nc [--output partition_plot.png]

@copyright © 2026 National Weather Service, National Oceanic and Atmospheric
Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
Weather Service.
NWS often uses Generative AI (GenAI) for code development and refactoring.
Whenever GenAI is used, NWS requires a full human review of code before it is
added to its repositories.
@author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
@author Contributors: Jules (Agentic AI)
@author Code Heritage: Converted and adapted from WAVEWATCH III (WW3) ww3_gspl
         by Hendrik L. Tolman and the WAVEWATCH III Development Group.
@date Initial, 2026-10-14
@date Last update : 2026-10-14
"""

import argparse
import struct
import sys
from pathlib import Path
from typing import Any, Tuple


def read_netcdf_partition_map(file_path: Path) -> Tuple[int, int, int, list[int]]:
    """
    Read partition map data and dimensions from a NetCDF (CDF-1/CDF-2) file.

    Parameters
    ----------
    file_path : Path
        Path to the input NetCDF file.

    Returns
    -------
    Tuple[int, int, int, list[int]]
        Tuple containing (nx, ny, num_partitions, partition_map_flat).
    """
    with open(file_path, "rb") as f:
        magic = f.read(4)
        if magic[:3] != b"CDF" or magic[3] not in (1, 2):
            raise ValueError(f"Invalid NetCDF file magic: {magic!r}")

        # Read numrecs
        _ = struct.unpack(">I", f.read(4))[0]

        # Read dimensions
        dim_tag = struct.unpack(">I", f.read(4))[0]
        nx, ny = 0, 0

        if dim_tag == 0x0000000A:
            dim_cnt = struct.unpack(">I", f.read(4))[0]
            for _ in range(dim_cnt):
                name_len = struct.unpack(">I", f.read(4))[0]
                dname = f.read(name_len).decode("ascii")
                pad = (4 - (name_len % 4)) % 4
                if pad:
                    f.read(pad)
                dlen = struct.unpack(">I", f.read(4))[0]
                if dname == "x":
                    nx = dlen
                elif dname == "y":
                    ny = dlen
        elif dim_tag == 0:
            _ = struct.unpack(">I", f.read(4))[0]

        # Read global attributes
        att_tag = struct.unpack(">I", f.read(4))[0]
        num_partitions = 1

        if att_tag == 0x0000000C:
            att_cnt = struct.unpack(">I", f.read(4))[0]
            for _ in range(att_cnt):
                name_len = struct.unpack(">I", f.read(4))[0]
                aname = f.read(name_len).decode("ascii")
                pad = (4 - (name_len % 4)) % 4
                if pad:
                    f.read(pad)
                atype, alen = struct.unpack(">II", f.read(8))
                if atype == 2:  # NC_CHAR
                    if alen > 0:
                        f.read(alen)
                    pad = (4 - (alen % 4)) % 4
                    if pad:
                        f.read(pad)
                elif atype == 4:  # NC_INT
                    ivals = struct.unpack(f">{alen}I", f.read(alen * 4))
                    if aname == "num_partitions" and alen > 0:
                        num_partitions = ivals[0]
        elif att_tag == 0:
            _ = struct.unpack(">I", f.read(4))[0]

        # Read variables
        var_tag = struct.unpack(">I", f.read(4))[0]
        pm_offset = 0

        if var_tag == 0x0000000B:
            var_cnt = struct.unpack(">I", f.read(4))[0]
            for _ in range(var_cnt):
                name_len = struct.unpack(">I", f.read(4))[0]
                vname = f.read(name_len).decode("ascii")
                pad = (4 - (name_len % 4)) % 4
                if pad:
                    f.read(pad)
                ndims = struct.unpack(">I", f.read(4))[0]
                _ = struct.unpack(f">{ndims}I", f.read(ndims * 4))

                vatag = struct.unpack(">I", f.read(4))[0]
                if vatag == 0x0000000C:
                    vacnt = struct.unpack(">I", f.read(4))[0]
                    for _ in range(vacnt):
                        vaname_len = struct.unpack(">I", f.read(4))[0]
                        _ = f.read(vaname_len)
                        vapad = (4 - (vaname_len % 4)) % 4
                        if vapad:
                            f.read(vapad)
                        vatype, valen = struct.unpack(">II", f.read(8))
                        if vatype == 2:
                            if valen > 0:
                                f.read(valen)
                            vapad = (4 - (valen % 4)) % 4
                            if vapad:
                                f.read(vapad)
                        elif vatype == 4:
                            f.read(valen * 4)
                elif vatag == 0:
                    _ = struct.unpack(">I", f.read(4))[0]

                _vtype, _vsize, voffset = struct.unpack(">III", f.read(12))
                if vname == "partition_map":
                    pm_offset = voffset

        if nx <= 0 or ny <= 0 or pm_offset == 0:
            raise ValueError(
                f"Failed to locate partition_map variable (nx={nx}, ny={ny}, offset={pm_offset})"
            )

        f.seek(pm_offset)
        raw_data = f.read(nx * ny * 4)
        partition_map = list(struct.unpack(f">{nx * ny}I", raw_data))

    return nx, ny, num_partitions, partition_map


def plot_partition_map(
    nx: int,
    ny: int,
    num_partitions: int,
    partition_map: list[int],
    output_path: Path | None = None,
) -> Any:
    """
    Plot the 2D domain partitioning grid using matplotlib.

    Parameters
    ----------
    nx : int
        Number of grid columns.
    ny : int
        Number of grid rows.
    num_partitions : int
        Number of partitions.
    partition_map : list[int]
        Flat 1D list of partition IDs (size nx * ny).
    output_path : Path | None, optional
        Path to save plot image file, by default None.
    """
    import matplotlib.colors as mcolors
    import matplotlib.pyplot as plt

    # Reshape 1D list into 2D grid [ny, nx]
    grid = [
        partition_map[y * nx : (y + 1) * nx]
        for y in range(ny)
    ]

    fig, ax = plt.subplots(figsize=(10, 6))

    cmap = plt.cm.get_cmap("tab20", num_partitions + 1)
    bounds = list(range(num_partitions + 2))
    norm = mcolors.BoundaryNorm(bounds, cmap.N)

    im = ax.imshow(
        grid,
        origin="lower",
        cmap=cmap,
        norm=norm,
        aspect="equal",
        interpolation="nearest",
    )

    cbar = fig.colorbar(im, ax=ax, ticks=[i + 0.5 for i in range(num_partitions + 1)])
    labels = ["0 (Land)"] + [f"Domain {i}" for i in range(1, num_partitions + 1)]
    cbar.ax.set_yticklabels(labels)

    ax.set_title(
        f"WAVEWATCH IV Domain Partitioning Map ({num_partitions} Partitions, {nx}x{ny} Grid)",
        fontsize=12,
        pad=12,
    )
    ax.set_xlabel("Longitude Index (X)", fontsize=10)
    ax.set_ylabel("Latitude Index (Y)", fontsize=10)
    ax.grid(True, which="both", color="gray", linestyle=":", linewidth=0.5)

    plt.tight_layout()

    if output_path:
        plt.savefig(output_path, dpi=300)
        print(f"Partition map plot saved to: {output_path}")
    else:
        plt.show()

    return fig


def main() -> None:
    """Main CLI entry point for partitioning map visualization."""
    parser = argparse.ArgumentParser(
        description="Display WAVEWATCH IV grid partitioning map from a NetCDF file."
    )
    parser.add_argument(
        "--file",
        type=Path,
        required=True,
        help="Path to input NetCDF (.nc) partitioning map file.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=None,
        help="Optional output image file path (e.g. partition_grid.png).",
    )

    args = parser.parse_args()

    if not args.file.exists():
        print(f"Error: Input file '{args.file}' does not exist.", file=sys.stderr)
        sys.exit(1)

    try:
        nx, ny, num_partitions, pmap = read_netcdf_partition_map(args.file)
        print(
            f"Successfully read NetCDF grid: nx={nx}, ny={ny}, num_partitions={num_partitions}"
        )
        plot_partition_map(nx, ny, num_partitions, pmap, args.output)
    except Exception as e:
        print(f"Error processing NetCDF file: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
