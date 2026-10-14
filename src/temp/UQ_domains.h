/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file UQ_domains.h
 * @brief Domain partitioning routines for ultimate quickest (UQ) grids in
 * WAVEWATCH IV (WW4).
 * @details Provides functions to partition global UQ structured grids into
 * sub-domains based on active sea points (derived from WW3 ww3_gspl).
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
 * @date Initial, 2026-10-14
 * @date Last update : 2026-10-14
 */

#ifndef UQ_DOMAINS_H
#define UQ_DOMAINS_H

#include <span>
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

} // namespace ww4_utils

#endif // UQ_DOMAINS_H
