/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file UQ_grid_mapping.cpp
 * @brief 2D spatial grid to 1D wet points and partition mapping routines for
 * ultimate quickest (UQ) grids in WAVEWATCH IV (WW4).
 * @details Provides pure subroutines and data structures to map between 2D
 * spatial grid coordinates (x, y) and 1D storage arrays considering only wet
 * (active sea) points, and to create mapping data between full domain storage
 * points and local partition storage points (derived from WW3 w3gridmd and
 * ww3_gspl).
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
 * w3gridmd and ww3_gspl by Hendrik L. Tolman.
 * @date Initial, 2026-11-03
 * @date Last update : 2026-11-03
 */

#include "temp/UQ_grid_mapping.h"

#include <algorithm>
#include <span>
#include <vector>

namespace ww4_utils {
namespace UQ_utils {

// --- createGridPointMapping -------------------------------------------------

GridPointMapping createGridPointMapping(int nx, int ny,
                                        std::span<const int> land_sea_map) {
  // === Section: Input Validation & Allocation Setup =========================
  GridPointMapping mapping;
  mapping.nx = nx;
  mapping.ny = ny;
  mapping.num_sea_points = 0;

  if (nx <= 0 || ny <= 0 ||
      land_sea_map.size() != static_cast<size_t>(nx * ny)) {
    return mapping;
  }

  const size_t total_cells = static_cast<size_t>(nx * ny);
  mapping.map_2d_to_1d.assign(total_cells, -1);

  // === Section: Active Wet Point Counting & Index Generation ================
  int sea_count = 0;
  for (int y = 0; y < ny; ++y) {
    for (int x = 0; x < nx; ++x) {
      int idx = y * nx + x;
      if (land_sea_map[idx] > 0) {
        mapping.map_2d_to_1d[idx] = sea_count;
        mapping.map_1d_to_2d.push_back(Point2D{x, y, idx});
        sea_count++;
      }
    }
  }

  mapping.num_sea_points = sea_count;
  return mapping;
}

// --- createPartitionMapping -------------------------------------------------

PartitionMapping createPartitionMapping(int nx, int ny,
                                        std::span<const int> land_sea_map,
                                        std::span<const int> partition_map,
                                        int target_partition,
                                        const GridPointMapping &full_mapping) {
  // === Section: Input Validation & Mapping Initialization ===================
  PartitionMapping pmap;
  pmap.target_partition = target_partition;
  pmap.num_local_sea_points = 0;
  pmap.num_global_sea_points = 0;

  if (nx <= 0 || ny <= 0 || target_partition <= 0 ||
      land_sea_map.size() != static_cast<size_t>(nx * ny) ||
      partition_map.size() != static_cast<size_t>(nx * ny)) {
    return pmap;
  }

  // Use precomputed full_mapping if valid, otherwise compute locally
  GridPointMapping local_grid_map;
  const GridPointMapping *active_mapping = &full_mapping;
  if (full_mapping.nx != nx || full_mapping.ny != ny ||
      full_mapping.map_2d_to_1d.size() != static_cast<size_t>(nx * ny)) {
    local_grid_map = createGridPointMapping(nx, ny, land_sea_map);
    active_mapping = &local_grid_map;
  }

  const int total_global_sea = active_mapping->num_sea_points;
  pmap.num_global_sea_points = total_global_sea;
  pmap.global_to_local_sea.assign(static_cast<size_t>(total_global_sea), -1);

  // === Section: Partition Wet Point Correlation =============================
  int local_sea_count = 0;
  for (int global_isea = 0; global_isea < total_global_sea; ++global_isea) {
    const Point2D &pt = active_mapping->map_1d_to_2d[global_isea];
    int cell_partition = partition_map[pt.global_linear_index];

    if (cell_partition == target_partition) {
      pmap.local_to_global_sea.push_back(global_isea);
      pmap.global_to_local_sea[global_isea] = local_sea_count;
      local_sea_count++;
    }
  }

  pmap.num_local_sea_points = local_sea_count;
  return pmap;
}

// --- createPartitionMapping (ReducedGrid Overload) --------------------------

PartitionMapping createPartitionMapping(const FullGrid &full_grid,
                                        const ReducedGrid &reduced_grid,
                                        const GridPointMapping &full_mapping) {
  // === Section: Input Validation & Mapping Initialization ===================
  PartitionMapping pmap;
  pmap.target_partition = reduced_grid.target_partition;
  pmap.num_local_sea_points = 0;
  pmap.num_global_sea_points = 0;

  const int nx = full_grid.nx;
  const int ny = full_grid.ny;

  if (nx <= 0 || ny <= 0 || reduced_grid.target_partition <= 0 ||
      reduced_grid.nx_local <= 0 || reduced_grid.ny_local <= 0 ||
      full_grid.land_sea_map.size() != static_cast<size_t>(nx * ny)) {
    return pmap;
  }

  // Ensure full_mapping is valid for this full_grid
  GridPointMapping local_grid_map;
  const GridPointMapping *active_mapping = &full_mapping;
  if (full_mapping.nx != nx || full_mapping.ny != ny ||
      full_mapping.map_2d_to_1d.size() != static_cast<size_t>(nx * ny)) {
    local_grid_map = createGridPointMapping(nx, ny, full_grid.land_sea_map);
    active_mapping = &local_grid_map;
  }

  const int total_global_sea = active_mapping->num_sea_points;
  pmap.num_global_sea_points = total_global_sea;
  pmap.global_to_local_sea.assign(static_cast<size_t>(total_global_sea), -1);

  // === Section: Reduced Grid Scan & Global Sea Correlation ==================
  const int nx_loc = reduced_grid.nx_local;
  const int ny_loc = reduced_grid.ny_local;
  const int x_start = reduced_grid.global_x_min;
  const int y_start = reduced_grid.global_y_min;

  int local_sea_count = 0;
  for (int y_l = 0; y_l < ny_loc; ++y_l) {
    int gy = y_start + y_l;
    if (gy < 0 || gy >= ny) {
      continue;
    }
    for (int x_l = 0; x_l < nx_loc; ++x_l) {
      int gx = (x_start + x_l) % nx;
      int loc_idx = y_l * nx_loc + x_l;

      // Only map sea points assigned to the target partition
      if (reduced_grid.partition_map[loc_idx] ==
          reduced_grid.target_partition) {
        int glob_idx = gy * nx + gx;
        int global_isea = active_mapping->map_2d_to_1d[glob_idx];

        if (global_isea >= 0 && pmap.global_to_local_sea[global_isea] == -1) {
          pmap.local_to_global_sea.push_back(global_isea);
          pmap.global_to_local_sea[global_isea] = local_sea_count;
          local_sea_count++;
        }
      }
    }
  }

  pmap.num_local_sea_points = local_sea_count;
  return pmap;
}

} // namespace UQ_utils
} // namespace ww4_utils
