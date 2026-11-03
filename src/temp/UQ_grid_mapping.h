/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file UQ_grid_mapping.h
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

#pragma once

#include <span>
#include <vector>

#include "temp/UQ_one_domain.h"

namespace ww4_utils {
namespace UQ_utils {

// --- Point2D ----------------------------------------------------------------
/**
 * @struct Point2D
 * @brief Discrete 2D spatial coordinates and linear index of a grid cell.
 * @details Encapsulates 0-based column (x), row (y), and row-major 2D array
 * offset (linear index = y * nx + x) for a cell in structured grids.
 */
struct Point2D {
  int x{0};                   ///< 0-based column index (longitude direction).
  int y{0};                   ///< 0-based row index (latitude direction).
  int global_linear_index{0}; ///< Row-major 2D index (y * nx + x).
};

// --- GridPointMapping -------------------------------------------------------
/**
 * @struct GridPointMapping
 * @brief Bidirectional mapping dataset between 2D spatial grid cells and 1D
 * storage arrays containing only active wet (sea) points.
 * @details Emulates the WW3 storage grid mapping mechanism (MAPFS and MAPSF).
 * In WAVEWATCH wave models, spectra and dynamic wave variables are stored
 * exclusively for active wet points in a 1D storage vector of length
 * num_sea_points (NSEA).
 */
struct GridPointMapping {
  int nx{0};             ///< Number of columns in 2D grid.
  int ny{0};             ///< Number of rows in 2D grid.
  int num_sea_points{0}; ///< Total number of active wet (sea) points (NSEA).
  std::vector<int>
      map_2d_to_1d; ///< 2D grid to 1D storage index (row-major order, size
                    ///< nx * ny). Maps (y * nx + x) to 0-based 1D sea index,
                    ///< or -1 for excluded/land points (analogous to MAPFS).
  std::vector<Point2D>
      map_1d_to_2d; ///< 1D storage index to 2D grid cell coordinates (size
                    ///< num_sea_points). Maps 0-based 1D sea index to 2D
                    ///< coordinates (analogous to MAPSF).
};

// --- PartitionMapping -------------------------------------------------------
/**
 * @struct PartitionMapping
 * @brief Mapping dataset connecting 1D wet points of a full grid domain to 1D
 * wet points of a target local partition.
 * @details Facilitates data transfers, MPI halo exchanges, and domain
 * gathering/scattering between global domain storage vectors and localized
 * partition vectors.
 */
struct PartitionMapping {
  int target_partition{0};      ///< Target partition ID (1-based).
  int num_local_sea_points{0};  ///< Number of sea points in local partition.
  int num_global_sea_points{0}; ///< Number of sea points in full grid domain.
  std::vector<int>
      local_to_global_sea; ///< Maps 0-based local 1D sea index to 0-based full
                           ///< grid 1D sea index (size num_local_sea_points).
  std::vector<int>
      global_to_local_sea; ///< Maps 0-based full grid 1D sea index to 0-based
                           ///< local 1D sea index, or -1 if the point is not
                           ///< owned by the target partition (size
                           ///< num_global_sea_points).
};

// --- createGridPointMapping -------------------------------------------------
/**
 * @brief Generates bidirectional mapping between 2D spatial grid and 1D wet
 * points.
 * @details Scans 2D grid status mask (row-major order), counts active sea
 * cells (where land_sea_map > 0), assigns sequential 0-based storage indices to
 * wet cells while setting dry/land cells to -1, and constructs the inverse
 * lookup table.
 *
 * Algorithm and Lifecycle:
 * Input Validation:
 *   Verifies dimensions (nx > 0, ny > 0) and validates that land_sea_map size
 *   matches nx * ny. Returns empty mapping if invalid.
 *
 * 2D-to-1D and 1D-to-2D Mapping Construction:
 *   Iterates through row-major indices (y from 0 to ny-1, x from 0 to nx-1).
 *   For each cell with land_sea_map > 0:
 *     Assigns current wet counter isea to map_2d_to_1d[idx].
 *     Records Point2D{x, y, idx} into map_1d_to_2d[isea].
 *     Increments isea.
 *   For dry cells (land_sea_map <= 0):
 *     Sets map_2d_to_1d[idx] = -1.
 *
 * @param nx Longitudinal grid column count (x-dimension).
 * @param ny Latitudinal grid row count (y-dimension).
 * @param land_sea_map 2D status mask (row-major, size nx * ny, > 0 for sea, <=
 * 0 for land).
 * @return GridPointMapping structure containing bidirectional mapping arrays.
 */
GridPointMapping createGridPointMapping(int nx, int ny,
                                        std::span<const int> land_sea_map);

// --- createPartitionMapping -------------------------------------------------
/**
 * @brief Creates index mapping between full grid wet points and a partition.
 * @details Examines partition assignments of all global sea points and builds
 * bidirectional index translation tables between global 1D wet indices and
 * local 1D wet indices for target_partition.
 *
 * Algorithm and Lifecycle:
 * Input Validation:
 *   Validates grid dimensions, partition map size, and target partition ID.
 *   If full_mapping is unpopulated or mismatched, generates mapping on the fly.
 *
 * Partition Sea Point Identification:
 *   Iterates over all global sea points in full_mapping. For each sea point,
 *   checks if its partition_map value matches target_partition.
 *   If matched:
 *     Records global sea index into local_to_global_sea.
 *     Sets global_to_local_sea[global_isea] = local_isea.
 *     Increments local_isea.
 *   If not matched:
 *     Sets global_to_local_sea[global_isea] = -1.
 *
 * @param nx Longitudinal grid column count.
 * @param ny Latitudinal grid row count.
 * @param land_sea_map 2D status mask (row-major, size nx * ny).
 * @param partition_map 2D partition map array (row-major, size nx * ny).
 * @param target_partition Target 1-based partition ID to map.
 * @param full_mapping Precomputed GridPointMapping for the full grid.
 * @return PartitionMapping structure with index translation tables.
 */
PartitionMapping createPartitionMapping(int nx, int ny,
                                        std::span<const int> land_sea_map,
                                        std::span<const int> partition_map,
                                        int target_partition,
                                        const GridPointMapping &full_mapping);

// --- createPartitionMapping (ReducedGrid Overload) --------------------------
/**
 * @brief Creates index mapping between full grid wet points and a ReducedGrid.
 * @details Correlates active sea points within a sliced ReducedGrid sub-domain
 * partition (including halo overlap cells or exclusive partition interior) to
 * the corresponding 1D sea points in the full grid.
 *
 * Algorithm and Lifecycle:
 * Reduced Grid Traversal:
 *   Iterates over local row (y_loc) and column (x_loc) indices of reduced_grid.
 *   Computes wrapped global coordinates:
 *     gx = (reduced_grid.global_x_min + x_loc) % full_grid.nx
 *     gy = reduced_grid.global_y_min + y_loc
 *   Matches local cells owned by reduced_grid.target_partition against full
 *   grid 1D sea indices.
 *
 * Translation Vector Construction:
 *   Builds local_to_global_sea and global_to_local_sea vectors representing
 *   the partition's wet points.
 *
 * @param full_grid Full global grid structure.
 * @param reduced_grid Extracted sub-domain partition grid with local fields.
 * @param full_mapping Precomputed GridPointMapping for the full grid.
 * @return PartitionMapping structure for the reduced grid.
 */
PartitionMapping createPartitionMapping(const FullGrid &full_grid,
                                        const ReducedGrid &reduced_grid,
                                        const GridPointMapping &full_mapping);

} // namespace UQ_utils
} // namespace ww4_utils
