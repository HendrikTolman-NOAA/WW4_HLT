/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file rectilinear_write_partition.h
 * @brief NetCDF partition map writer tools for WAVEWATCH IV rectilinear grids.
 * @details Header declaring subroutines to open a NetCDF partition map file,
 * populate metadata from the full grid and partition configuration, and
 * incrementally write one or more partition maps to document map evolution.
 *
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-11-03
 * @date Last update : 2026-11-23
 */

#pragma once

#include "ww4_core/grid_rectilinear/utils_rectilinear_IO/rectilinear_read_grid.h"
#include <cstdint>
#include <filesystem>
#include <span>

namespace ww4::io {

// --- openRectilinearPartitionMapFile ----------------------------------------
/**
 * @brief Creates a new NetCDF partition map file and writes header and grid
 * metadata for the rectilinear grid.
 *
 * @details Creates a NetCDF file at `filepath` with an unlimited dimension
 * `n_partition_maps`, along with spatial grid dimensions `ny` and `nx`. Writes
 * all grid metadata and global attributes inherited from `grid`, sets
 * `num_partitions`, and defines the partition map variable. Leaves the file
 * open in data mode ready for writing map records.
 *
 * @param[in]  filepath       Destination file path.
 * @param[in]  grid           UgridGrid structure providing metadata and
 *                            dimensions.
 * @param[in]  num_partitions Total number of partitions created.
 * @param[out] ncid           Open NetCDF file ID returned upon success.
 * @return int Status code (0 = success, 1 = file creation failure, 2 = metadata
 * or definition failure).
 */
[[nodiscard]] int
openRectilinearPartitionMapFile(const std::filesystem::path &filepath,
                                const UgridGrid &grid, int num_partitions,
                                int &ncid);

// --- writeSingleRectilinearPartitionMap -------------------------------------
/**
 * @brief Appends a single partition map to an open partition map NetCDF file
 * for the rectilinear grid.
 *
 * @details Writes a 2D partition map array of size `ny * nx` into the open
 * NetCDF dataset at the record position designated by `map_index`. Upon
 * successful write, increments `map_index`.
 *
 * @param[in]     ncid          Open NetCDF file ID (from
 * openRectilinearPartitionMapFile).
 * @param[in]     partition_map 2D partition ID slice (size ny * nx).
 * @param[in,out] map_index     Zero-based record index to write; incremented on
 *                              success.
 * @return int Status code (0 = success, 3 = data writing failure).
 */
[[nodiscard]] int writeSingleRectilinearPartitionMap(
    int ncid, std::span<const int32_t> partition_map, std::size_t &map_index);

// --- closeRectilinearPartitionMapFile ---------------------------------------
/**
 * @brief Closes an open partition map NetCDF file for the rectilinear grid.
 *
 * @param[in] ncid Open NetCDF file ID.
 * @return int Status code (0 = success, non-zero on netcdf error).
 */
[[nodiscard]] int closeRectilinearPartitionMapFile(int ncid);

// --- writeRectilinearPartitionMapNetCDF -------------------------------------
/**
 * @brief Convenience one-shot routine to write a partition map file with a
 * single map for the rectilinear grid.
 *
 * @param[in] filepath       Destination file path.
 * @param[in] grid           UgridGrid structure providing metadata and
 *                           dimensions.
 * @param[in] num_partitions Total number of partitions created.
 * @param[in] partition_map  2D partition map slice (size ny * nx).
 * @return int Status code (0 = success, 1 = open failure, 2 = metadata failure,
 * 3 = data write failure).
 */
[[nodiscard]] int
writeRectilinearPartitionMapNetCDF(const std::filesystem::path &filepath,
                                   const UgridGrid &grid, int num_partitions,
                                   std::span<const int32_t> partition_map);

// Backward-compatible inline wrappers:
inline int openPartitionMapFile(const std::filesystem::path &filepath,
                                const UgridGrid &grid, int num_partitions,
                                int &ncid) {
  return openRectilinearPartitionMapFile(filepath, grid, num_partitions, ncid);
}

inline int writeSinglePartitionMap(int ncid,
                                   std::span<const int32_t> partition_map,
                                   std::size_t &map_index) {
  return writeSingleRectilinearPartitionMap(ncid, partition_map, map_index);
}

inline int closePartitionMapFile(int ncid) {
  return closeRectilinearPartitionMapFile(ncid);
}

inline int writePartitionMapNetCDF(const std::filesystem::path &filepath,
                                   const UgridGrid &grid, int num_partitions,
                                   std::span<const int32_t> partition_map) {
  return writeRectilinearPartitionMapNetCDF(filepath, grid, num_partitions,
                                            partition_map);
}

} // namespace ww4::io
