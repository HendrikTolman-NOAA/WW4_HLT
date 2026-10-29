/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file UQ_domains.cpp
 * @brief Domain partitioning routines for ultimate quickest (UQ) grids in
 * WAVEWATCH IV (WW4).
 * @details Provides pure subroutines to partition full UQ structured grids into
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
 * @date Last update : 2026-10-29
 */

#include "temp/UQ_domains.h"

#include <algorithm>
#include <cmath>
#include <span>
#include <vector>

namespace ww4_utils {
namespace UQ_utils {

namespace {

// --- countSeaPointsInRect ---------------------------------------------------
/**
 * @brief Counts active sea points in a rectangular bounding box.
 * @param x_min Minimum longitude index (0-based).
 * @param x_max Maximum longitude index (0-based).
 * @param y_min Minimum latitude index (0-based).
 * @param y_max Maximum latitude index (0-based).
 * @param nx Number of longitudinal grid columns.
 * @param land_sea_map 2D land-sea mask span.
 * @return Count of sea points (where land_sea_map > 0).
 */
int countSeaPointsInRect(int x_min, int x_max, int y_min, int y_max, int nx,
                         std::span<const int> land_sea_map) {
  int count = 0;
  for (int y = y_min; y <= y_max; ++y) {
    for (int x = x_min; x <= x_max; ++x) {
      if (land_sea_map[y * nx + x] > 0) {
        count++;
      }
    }
  }
  return count;
}

// --- recursiveBisectDomain --------------------------------------------------
/**
 * @brief Recursively bisects a domain bounding box into target sub-partitions.
 * @param x_min Minimum longitude index (0-based).
 * @param x_max Maximum longitude index (0-based).
 * @param y_min Minimum latitude index (0-based).
 * @param y_max Maximum latitude index (0-based).
 * @param nx Number of longitudinal grid columns.
 * @param land_sea_map 2D land-sea mask span.
 * @param target_partitions Number of partitions to divide into.
 * @param partition_start_id Starting partition ID for this sub-block.
 * @param[out] partition_map Output partition map vector to update.
 */
void recursiveBisectDomain(int x_min, int x_max, int y_min, int y_max, int nx,
                           std::span<const int> land_sea_map,
                           int target_partitions, int partition_start_id,
                           std::vector<int> &partition_map) {
  int total_sea =
      countSeaPointsInRect(x_min, x_max, y_min, y_max, nx, land_sea_map);
  if (total_sea == 0) {
    return;
  }

  if (target_partitions <= 1) {
    for (int y = y_min; y <= y_max; ++y) {
      for (int x = x_min; x <= x_max; ++x) {
        int idx = y * nx + x;
        if (land_sea_map[idx] > 0) {
          partition_map[idx] = partition_start_id;
        }
      }
    }
    return;
  }

  int p1 = target_partitions / 2;
  int p2 = target_partitions - p1;

  int target_s1 = static_cast<int>(
      std::round(static_cast<double>(total_sea) * static_cast<double>(p1) /
                 static_cast<double>(target_partitions)));
  target_s1 = std::clamp(target_s1, 1, total_sea - 1);

  bool split_x = (x_max - x_min >= y_max - y_min);

  if (split_x && x_max > x_min) {
    int current_sea = 0;
    int best_x = x_min;
    int best_diff = total_sea + 1;

    for (int x = x_min; x < x_max; ++x) {
      int sea_in_col =
          countSeaPointsInRect(x, x, y_min, y_max, nx, land_sea_map);
      current_sea += sea_in_col;
      int diff = std::abs(current_sea - target_s1);
      if (diff < best_diff) {
        best_diff = diff;
        best_x = x;
      }
    }

    recursiveBisectDomain(x_min, best_x, y_min, y_max, nx, land_sea_map, p1,
                          partition_start_id, partition_map);
    recursiveBisectDomain(best_x + 1, x_max, y_min, y_max, nx, land_sea_map, p2,
                          partition_start_id + p1, partition_map);
  } else if (y_max > y_min) {
    int current_sea = 0;
    int best_y = y_min;
    int best_diff = total_sea + 1;

    for (int y = y_min; y < y_max; ++y) {
      int sea_in_row =
          countSeaPointsInRect(x_min, x_max, y, y, nx, land_sea_map);
      current_sea += sea_in_row;
      int diff = std::abs(current_sea - target_s1);
      if (diff < best_diff) {
        best_diff = diff;
        best_y = y;
      }
    }

    recursiveBisectDomain(x_min, x_max, y_min, best_y, nx, land_sea_map, p1,
                          partition_start_id, partition_map);
    recursiveBisectDomain(x_min, x_max, best_y + 1, y_max, nx, land_sea_map, p2,
                          partition_start_id + p1, partition_map);
  } else {
    for (int y = y_min; y <= y_max; ++y) {
      for (int x = x_min; x <= x_max; ++x) {
        int idx = y * nx + x;
        if (land_sea_map[idx] > 0) {
          partition_map[idx] = partition_start_id;
        }
      }
    }
  }
}

// --- refinePartitionsIteratively --------------------------------------------
/**
 * @brief Iteratively refines partition assignments at domain boundaries to
 * balance active sea point counts across partitions.
 * @details Examines boundary sea points between overloaded and underloaded
 * adjacent partitions and reassigns points to improve overall load balance
 * until convergence or max_iterations is reached.
 * @param nx Number of longitudinal grid columns.
 * @param ny Number of latitudinal grid rows.
 * @param land_sea_map 2D land-sea mask span.
 * @param num_partitions Total number of target partitions.
 * @param max_iterations Maximum number of boundary swap passes.
 * @param[in,out] partition_map Partition IDs vector to refine in place.
 */
void refinePartitionsIteratively(int nx, int ny,
                                 std::span<const int> land_sea_map,
                                 int num_partitions, int max_iterations,
                                 std::vector<int> &partition_map) {
  if (num_partitions <= 1 || max_iterations <= 0) {
    return;
  }

  std::vector<int> partition_counts(num_partitions + 1, 0);
  int total_sea = 0;
  for (size_t i = 0; i < partition_map.size(); ++i) {
    int pid = partition_map[i];
    if (land_sea_map[i] > 0 && pid >= 1 && pid <= num_partitions) {
      partition_counts[pid]++;
      total_sea++;
    }
  }

  if (total_sea == 0) {
    return;
  }

  double target_per_partition = static_cast<double>(total_sea) / num_partitions;

  const int dx[] = {0, 0, -1, 1};
  const int dy[] = {-1, 1, 0, 0};

  for (int iter = 0; iter < max_iterations; ++iter) {
    int swaps_made = 0;

    for (int y = 0; y < ny; ++y) {
      for (int x = 0; x < nx; ++x) {
        int idx = y * nx + x;
        if (land_sea_map[idx] <= 0) {
          continue;
        }

        int p1 = partition_map[idx];
        if (p1 < 1 || p1 > num_partitions) {
          continue;
        }

        if (partition_counts[p1] <=
            static_cast<int>(std::floor(target_per_partition))) {
          continue;
        }

        for (int k = 0; k < 4; ++k) {
          int nx_nbr = x + dx[k];
          int ny_nbr = y + dy[k];

          if (nx_nbr < 0 || nx_nbr >= nx || ny_nbr < 0 || ny_nbr >= ny) {
            continue;
          }

          int nbr_idx = ny_nbr * nx + nx_nbr;
          if (land_sea_map[nbr_idx] <= 0) {
            continue;
          }

          int p2 = partition_map[nbr_idx];
          if (p2 < 1 || p2 > num_partitions || p2 == p1) {
            continue;
          }

          if (partition_counts[p2] <
              static_cast<int>(std::ceil(target_per_partition))) {
            partition_map[idx] = p2;
            partition_counts[p1]--;
            partition_counts[p2]++;
            swaps_made++;
            break;
          }
        }
      }
    }

    if (swaps_made == 0) {
      break;
    }
  }
}

} // namespace

// --- partitionGlobalGrid ----------------------------------------------------

std::vector<int> partitionGlobalGrid(int nx, int ny,
                                     std::span<const int> land_sea_map,
                                     int num_partitions, int max_iterations) {
  if (nx <= 0 || ny <= 0 || num_partitions <= 0 ||
      land_sea_map.size() != static_cast<size_t>(nx * ny)) {
    return {};
  }

  std::vector<int> partition_map(nx * ny, 0);

  recursiveBisectDomain(0, nx - 1, 0, ny - 1, nx, land_sea_map, num_partitions,
                        1, partition_map);

  if (num_partitions > 1 && max_iterations > 0) {
    refinePartitionsIteratively(nx, ny, land_sea_map, num_partitions,
                                max_iterations, partition_map);
  }

  return partition_map;
}

} // namespace UQ_utils
} // namespace ww4_utils
