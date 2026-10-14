/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_domain.h
 * @brief Domain partitioning and NetCDF I/O routines for WAVEWATCH IV (WW4).
 * @details Provides functions to partition global UQ structured grids into
 * sub-domains based on active sea points (derived from WW3 ww3_gspl) and
 * routines to write and read partitioning maps in NetCDF format.
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
 * ww3_gspl by Hendrik L. Tolman and the WAVEWATCH III Development Group.
 * @date Initial, 2026-10-14
 * @date Last update : 2026-10-14
 */

#ifndef WW4_DOMAIN_H
#define WW4_DOMAIN_H

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace ww4_utils {

/**
 * @struct DomainPartition
 * @brief Structure representing a partitioned grid domain dataset.
 */
struct DomainPartition {
  int nx{0};
  int ny{0};
  int num_partitions{0};
  std::vector<int> partition_map;
  std::vector<int> land_sea_map;
};

/**
 * @brief Partitions a global UQ rectangular grid into sub-domains based on
 * active sea points.
 * @details Implements recursive bisection (derived from WW3 ww3_gspl) to
 * distribute sea points evenly across target partitions while forming spatially
 * contiguous sub-domain blocks.
 * @param nx Number of longitudinal grid columns (x-dimension).
 * @param ny Number of latitudinal grid rows (y-dimension).
 * @param land_sea_map 2D grid status mask (row-major, size nx * ny), where
 * values > 0 indicate sea points and <= 0 indicate land points.
 * @param num_partitions Total number of target partitions (must be >= 1).
 * @return Vector of size nx * ny containing partition IDs (1 to num_partitions
 * for sea points, 0 for land).
 */
std::vector<int> partitionGlobalGrid(int nx, int ny,
                                     std::span<const int> land_sea_map,
                                     int num_partitions);

/**
 * @brief Writes a grid partitioning map and optional land-sea map to a NetCDF
 * file (.nc).
 * @details Encodes grid dimensions (nx, ny), metadata attributes, and
 * partitioning map data into a standard NetCDF format file.
 * @param filename Output NetCDF file path.
 * @param nx Number of longitudinal grid columns.
 * @param ny Number of latitudinal grid rows.
 * @param partition_map Partition IDs array (size nx * ny).
 * @param num_partitions Total number of partitions.
 * @param land_sea_map Optional land-sea status mask array (size nx * ny).
 * @return True if file was successfully written, false otherwise.
 */
bool writePartitionMapNetCDF(std::string_view filename, int nx, int ny,
                             std::span<const int> partition_map,
                             int num_partitions,
                             std::span<const int> land_sea_map = {});

/**
 * @brief Reads a grid partitioning map from a NetCDF file (.nc).
 * @details Parses NetCDF binary file structure to extract grid dimensions (nx,
 * ny), number of partitions, and the 2D partition map array.
 * @param filename Input NetCDF file path.
 * @param[out] nx Extracted number of longitudinal grid columns.
 * @param[out] ny Extracted number of latitudinal grid rows.
 * @param[out] num_partitions Extracted number of partitions.
 * @param[out] partition_map Extracted partition map vector (size nx * ny).
 * @return True if file was successfully read and parsed, false otherwise.
 */
bool readPartitionMapNetCDF(std::string_view filename, int &nx, int &ny,
                            int &num_partitions,
                            std::vector<int> &partition_map);

} // namespace ww4_utils

#endif // WW4_DOMAIN_H
