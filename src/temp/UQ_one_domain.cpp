/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file UQ_one_domain.cpp
 * @brief Subdomain extraction routine for ultimate quickest (UQ) grids in
 * WAVEWATCH IV (WW4).
 * @details Provides a pure subroutine to extract a reduced grid domain
 * partition including halo cells and associated grid fields for a target
 * partition (derived from WW3 ww3_gspl).
 *
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @author Code Heritage: Converted and adapted from WAVEWATCH III (WW3)
 * ww3_gspl by Hendrik L. Tolman.
 * @date Initial, 2026-10-29
 * @date Last update : 2026-10-29
 */

#include "temp/UQ_one_domain.h"

#include <algorithm>
#include <span>
#include <vector>

namespace ww4_utils {
namespace UQ_utils {

// --- extractOneDomain -------------------------------------------------------

ReducedGrid extractOneDomain(const FullGrid &full_grid,
                             std::span<const int> partition_map,
                             int target_partition, int halo_width) {
  ReducedGrid reduced;
  reduced.target_partition = target_partition;
  reduced.halo_width = std::max(0, halo_width);

  const int nx = full_grid.nx;
  const int ny = full_grid.ny;

  // Validate grid dimensions, partition map size, and target partition ID
  if (nx <= 0 || ny <= 0 || target_partition <= 0 ||
      partition_map.size() != static_cast<size_t>(nx * ny)) {
    return reduced;
  }

  // === Section: Bounding Box Search =========================================
  // Locate minimum and maximum global x and y coordinates for cells assigned
  // to target_partition.
  int x_min = nx, x_max = -1, y_min = ny, y_max = -1;

  for (int y = 0; y < ny; ++y) {
    for (int x = 0; x < nx; ++x) {
      int idx = y * nx + x;
      if (partition_map[idx] == target_partition) {
        x_min = std::min(x_min, x);
        x_max = std::max(x_max, x);
        y_min = std::min(y_min, y);
        y_max = std::max(y_max, y);
      }
    }
  }

  // Return empty reduced grid if target partition ID was not found
  if (x_max < x_min || y_max < y_min) {
    return reduced;
  }

  // === Section: Halo Expansion & Boundary Clamping ==========================
  // Expand bounding box by halo_width, clamping to global grid boundaries [0,
  // nx-1] and [0, ny-1].
  int x_start = std::max(0, x_min - reduced.halo_width);
  int x_end = std::min(nx - 1, x_max + reduced.halo_width);
  int y_start = std::max(0, y_min - reduced.halo_width);
  int y_end = std::min(ny - 1, y_max + reduced.halo_width);

  reduced.global_x_min = x_start;
  reduced.global_y_min = y_start;
  reduced.global_x_max = x_end;
  reduced.global_y_max = y_end;

  reduced.nx_local = x_end - x_start + 1;
  reduced.ny_local = y_end - y_start + 1;

  const int nx_loc = reduced.nx_local;
  const int ny_loc = reduced.ny_local;
  const size_t total_local_cells = static_cast<size_t>(nx_loc * ny_loc);

  // === Section: 2D Field Slicing ============================================
  // Slices land-sea mask and partition map
  reduced.land_sea_map.resize(total_local_cells, 0);
  reduced.partition_map.resize(total_local_cells, 0);

  const bool has_ls =
      (full_grid.land_sea_map.size() == static_cast<size_t>(nx * ny));

  for (int y_loc = 0; y_loc < ny_loc; ++y_loc) {
    int gy = y_start + y_loc;
    for (int x_loc = 0; x_loc < nx_loc; ++x_loc) {
      int gx = x_start + x_loc;
      int g_idx = gy * nx + gx;
      int l_idx = y_loc * nx_loc + x_loc;

      reduced.partition_map[l_idx] = partition_map[g_idx];
      if (has_ls) {
        reduced.land_sea_map[l_idx] = full_grid.land_sea_map[g_idx];
      }
    }
  }

  // Slice topobathy elevation field
  if (full_grid.topobathy.size() == static_cast<size_t>(nx * ny)) {
    reduced.topobathy.resize(total_local_cells, 0.0);
    for (int y_loc = 0; y_loc < ny_loc; ++y_loc) {
      int gy = y_start + y_loc;
      for (int x_loc = 0; x_loc < nx_loc; ++x_loc) {
        int gx = x_start + x_loc;
        reduced.topobathy[y_loc * nx_loc + x_loc] =
            full_grid.topobathy[gy * nx + gx];
      }
    }
  }

  // Slice subgrid obstruction fields X and Y
  if (full_grid.subgrid_obs_x.size() == static_cast<size_t>(nx * ny)) {
    reduced.subgrid_obs_x.resize(total_local_cells, 0.0);
    for (int y_loc = 0; y_loc < ny_loc; ++y_loc) {
      int gy = y_start + y_loc;
      for (int x_loc = 0; x_loc < nx_loc; ++x_loc) {
        int gx = x_start + x_loc;
        reduced.subgrid_obs_x[y_loc * nx_loc + x_loc] =
            full_grid.subgrid_obs_x[gy * nx + gx];
      }
    }
  }

  if (full_grid.subgrid_obs_y.size() == static_cast<size_t>(nx * ny)) {
    reduced.subgrid_obs_y.resize(total_local_cells, 0.0);
    for (int y_loc = 0; y_loc < ny_loc; ++y_loc) {
      int gy = y_start + y_loc;
      for (int x_loc = 0; x_loc < nx_loc; ++x_loc) {
        int gx = x_start + x_loc;
        reduced.subgrid_obs_y[y_loc * nx_loc + x_loc] =
            full_grid.subgrid_obs_y[gy * nx + gx];
      }
    }
  }

  // === Section: Spatial Coordinate Slicing ==================================
  // Slice longitude (1D vector of size nx or 2D matrix of size nx * ny)
  if (full_grid.longitude.size() == static_cast<size_t>(nx)) {
    reduced.longitude.resize(nx_loc);
    for (int x_loc = 0; x_loc < nx_loc; ++x_loc) {
      reduced.longitude[x_loc] = full_grid.longitude[x_start + x_loc];
    }
  } else if (full_grid.longitude.size() == static_cast<size_t>(nx * ny)) {
    reduced.longitude.resize(total_local_cells);
    for (int y_loc = 0; y_loc < ny_loc; ++y_loc) {
      int gy = y_start + y_loc;
      for (int x_loc = 0; x_loc < nx_loc; ++x_loc) {
        int gx = x_start + x_loc;
        reduced.longitude[y_loc * nx_loc + x_loc] =
            full_grid.longitude[gy * nx + gx];
      }
    }
  }

  // Slice latitude (1D vector of size ny or 2D matrix of size nx * ny)
  if (full_grid.latitude.size() == static_cast<size_t>(ny)) {
    reduced.latitude.resize(ny_loc);
    for (int y_loc = 0; y_loc < ny_loc; ++y_loc) {
      reduced.latitude[y_loc] = full_grid.latitude[y_start + y_loc];
    }
  } else if (full_grid.latitude.size() == static_cast<size_t>(nx * ny)) {
    reduced.latitude.resize(total_local_cells);
    for (int y_loc = 0; y_loc < ny_loc; ++y_loc) {
      int gy = y_start + y_loc;
      for (int x_loc = 0; x_loc < nx_loc; ++x_loc) {
        int gx = x_start + x_loc;
        reduced.latitude[y_loc * nx_loc + x_loc] =
            full_grid.latitude[gy * nx + gx];
      }
    }
  }

  return reduced;
}

} // namespace UQ_utils
} // namespace ww4_utils
