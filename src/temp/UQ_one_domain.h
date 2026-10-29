/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file UQ_one_domain.h
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

#pragma once

#include <span>
#include <vector>

namespace ww4_utils {
namespace UQ_utils {

// --- FullGrid ---------------------------------------------------------------
/**
 * @struct FullGrid
 * @brief Structure representing a full global grid dataset and associated
 * fields.
 */
struct FullGrid {
  int nx{0};
  int ny{0};
  bool is_global{false}; ///< Flag indicating whether grid has longitudinal
                         ///< closure (360-degree wrapping)
  std::vector<int>
      land_sea_map; ///< 2D land-sea mask (size nx * ny, >0 sea, <=0 land)
  std::vector<double>
      longitude; ///< 1D x-coordinates (size nx) or 2D (size nx * ny)
  std::vector<double>
      latitude; ///< 1D y-coordinates (size ny) or 2D (size nx * ny)
  std::vector<double>
      topobathy; ///< Unified topobathy elevation array (size nx * ny)
  std::vector<double>
      subgrid_obs_x; ///< Optional subgrid obstruction factors X (size nx * ny)
  std::vector<double>
      subgrid_obs_y; ///< Optional subgrid obstruction factors Y (size nx * ny)
};

// --- ReducedGrid ------------------------------------------------------------
/**
 * @struct ReducedGrid
 * @brief Structure representing a reduced sub-domain grid partition dataset.
 */
struct ReducedGrid {
  int target_partition{0}; ///< Target partition ID (1-based)
  int nx_local{0};         ///< Local grid column count including halo
  int ny_local{0};         ///< Local grid row count including halo
  int global_x_min{0};     ///< 0-based global x index of local origin (0,0)
  int global_y_min{0};     ///< 0-based global y index of local origin (0,0)
  int global_x_max{
      0}; ///< 0-based global x index of local (nx_local-1, ny_local-1)
  int global_y_max{
      0}; ///< 0-based global y index of local (nx_local-1, ny_local-1)
  int halo_width{0};              ///< Halo overlap width in grid cells
  std::vector<int> land_sea_map;  ///< Sliced local land-sea status mask (size
                                  ///< nx_local * ny_local)
  std::vector<int> partition_map; ///< Sliced local partition map IDs (size
                                  ///< nx_local * ny_local)
  std::vector<double> longitude;  ///< Sliced local x-coords (size nx_local for
                                  ///< 1D, nx_local * ny_local for 2D)
  std::vector<double> latitude;   ///< Sliced local y-coords (size ny_local for
                                  ///< 1D, nx_local * ny_local for 2D)
  std::vector<double> topobathy;  ///< Sliced local topobathy elevation (size
                                  ///< nx_local * ny_local)
  std::vector<double> subgrid_obs_x; ///< Sliced local subgrid obstruction X
                                     ///< (size nx_local * ny_local)
  std::vector<double> subgrid_obs_y; ///< Sliced local subgrid obstruction Y
                                     ///< (size nx_local * ny_local)
};

// --- extractOneDomain -------------------------------------------------------
/**
 * @brief Extracts a single reduced sub-domain partition grid with halo cells.
 * @details Pure subroutine that identifies the bounding box of active sea
 * points assigned to target_partition in partition_map, expands the bounding
 * box by halo_width cells in all cardinal directions (clamped to global grid
 * boundaries), and slices all grid field datasets from full_grid into
 * reduced_grid.
 *
 * Bounding Box & Halo Extraction Algorithm (derived from WW3 ww3_gspl):
 * Bounding Box Search:
 *   Scans global grid (y, x) for points where partition_map[y * nx + x] ==
 * target_partition. Determines x_min, x_max, y_min, y_max bounding box
 * containing all cells of target_partition.
 *
 * Halo Expansion:
 *   Expands bounding box by halo_width:
 *     x_start = max(0, x_min - halo_width)
 *     x_end   = min(nx - 1, x_max + halo_width)
 *     y_start = max(0, y_min - halo_width)
 *     y_end   = min(ny - 1, y_max + halo_width)
 *   Local grid dimensions become:
 *     nx_local = x_end - x_start + 1
 *     ny_local = y_end - y_start + 1
 *
 * Data Slicing:
 *   Slices 2D grid fields (land_sea_map, partition_map, topobathy, subgrid_obs)
 *   from row-major global indices into row-major local indices.
 *   Slices coordinates (longitude, latitude) preserving 1D (vector) or 2D grid
 * form.
 *
 * @param full_grid Full global grid structure containing spatial dimensions and
 * grid fields.
 * @param partition_map Global 2D partition map array (size full_grid.nx *
 * full_grid.ny).
 * @param target_partition Target 1-based partition ID to extract.
 * @param halo_width Halo overlap width in grid cells (default: 1).
 * @return ReducedGrid structure containing extracted local sub-domain grid and
 * sliced data.
 */
ReducedGrid extractOneDomain(const FullGrid &full_grid,
                             std::span<const int> partition_map,
                             int target_partition, int halo_width = 1,
                             bool is_global = false);

} // namespace UQ_utils
} // namespace ww4_utils
