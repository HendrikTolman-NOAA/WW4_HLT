/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_UQ_grid_mapping.cpp
 * @brief Unit tests for UQ grid 2D-to-1D wet mapping and partition mapping
 * routines (createGridPointMapping, createPartitionMapping).
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

#include <gtest/gtest.h>

#include <numeric>
#include <vector>

#include "temp/UQ_domains.h"
#include "temp/UQ_grid_mapping.h"
#include "temp/UQ_one_domain.h"

using namespace ww4_utils;
using namespace ww4_utils::UQ_utils;

TEST(UQGridMappingTest, CreateGridPointMappingBasic) {
  const int nx = 4;
  const int ny = 3;
  // 4x3 grid with mixed land and sea
  // Row 0: sea (1), land (0), sea (1), sea (1)
  // Row 1: land (0), land (0), sea (1), sea (1)
  // Row 2: sea (1), sea (1), land (0), sea (1)
  std::vector<int> land_sea = {1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1};

  GridPointMapping mapping = createGridPointMapping(nx, ny, land_sea);

  EXPECT_EQ(mapping.nx, nx);
  EXPECT_EQ(mapping.ny, ny);
  // Total sea points = 1 + 0 + 1 + 1 + 0 + 0 + 1 + 1 + 1 + 1 + 0 + 1 = 8
  EXPECT_EQ(mapping.num_sea_points, 8);
  ASSERT_EQ(mapping.map_2d_to_1d.size(), static_cast<size_t>(nx * ny));
  ASSERT_EQ(mapping.map_1d_to_2d.size(), 8u);

  // Check land points are mapped to -1
  EXPECT_EQ(mapping.map_2d_to_1d[1], -1);  // (x=1, y=0)
  EXPECT_EQ(mapping.map_2d_to_1d[4], -1);  // (x=0, y=1)
  EXPECT_EQ(mapping.map_2d_to_1d[5], -1);  // (x=1, y=1)
  EXPECT_EQ(mapping.map_2d_to_1d[10], -1); // (x=2, y=2)

  // Verify bidirectional consistency for all sea points
  for (int isea = 0; isea < mapping.num_sea_points; ++isea) {
    const Point2D &pt = mapping.map_1d_to_2d[isea];
    int expected_idx = pt.y * nx + pt.x;
    EXPECT_EQ(pt.global_linear_index, expected_idx);
    EXPECT_EQ(mapping.map_2d_to_1d[expected_idx], isea);
    EXPECT_GT(land_sea[expected_idx], 0);
  }
}

TEST(UQGridMappingTest, CreateGridPointMappingAllLandAndAllSea) {
  // All land case
  std::vector<int> all_land(12, 0);
  GridPointMapping land_map = createGridPointMapping(4, 3, all_land);
  EXPECT_EQ(land_map.num_sea_points, 0);
  EXPECT_TRUE(land_map.map_1d_to_2d.empty());
  for (int v : land_map.map_2d_to_1d) {
    EXPECT_EQ(v, -1);
  }

  // All sea case
  std::vector<int> all_sea(12, 1);
  GridPointMapping sea_map = createGridPointMapping(4, 3, all_sea);
  EXPECT_EQ(sea_map.num_sea_points, 12);
  EXPECT_EQ(sea_map.map_1d_to_2d.size(), 12u);
  for (int i = 0; i < 12; ++i) {
    EXPECT_EQ(sea_map.map_2d_to_1d[i], i);
    EXPECT_EQ(sea_map.map_1d_to_2d[i].global_linear_index, i);
  }
}

TEST(UQGridMappingTest, CreateGridPointMappingInvalidDimensions) {
  std::vector<int> dummy(10, 1);
  GridPointMapping bad1 = createGridPointMapping(-1, 5, dummy);
  EXPECT_EQ(bad1.num_sea_points, 0);
  EXPECT_TRUE(bad1.map_2d_to_1d.empty());

  GridPointMapping bad2 = createGridPointMapping(2, 2, dummy); // size mismatch
  EXPECT_EQ(bad2.num_sea_points, 0);
  EXPECT_TRUE(bad2.map_2d_to_1d.empty());
}

TEST(UQGridMappingTest, CreatePartitionMappingBasic) {
  const int nx = 6;
  const int ny = 6;
  FullGrid full;
  full.nx = nx;
  full.ny = ny;
  full.land_sea_map.assign(nx * ny, 1);
  // Set some land points
  full.land_sea_map[0] = 0;
  full.land_sea_map[7] = 0;

  const int num_partitions = 2;
  std::vector<int> partition_map =
      partitionGlobalGrid(nx, ny, full.land_sea_map, num_partitions);

  GridPointMapping full_mapping =
      createGridPointMapping(nx, ny, full.land_sea_map);

  EXPECT_EQ(full_mapping.num_sea_points, 34); // 36 - 2 land

  // Create partition mapping for partition 1
  PartitionMapping pmap1 = createPartitionMapping(
      nx, ny, full.land_sea_map, partition_map, 1, full_mapping);

  EXPECT_EQ(pmap1.target_partition, 1);
  EXPECT_EQ(pmap1.num_global_sea_points, 34);
  EXPECT_GT(pmap1.num_local_sea_points, 0);
  EXPECT_EQ(pmap1.local_to_global_sea.size(),
            static_cast<size_t>(pmap1.num_local_sea_points));
  EXPECT_EQ(pmap1.global_to_local_sea.size(), 34u);

  // Create partition mapping for partition 2
  PartitionMapping pmap2 = createPartitionMapping(
      nx, ny, full.land_sea_map, partition_map, 2, full_mapping);

  EXPECT_EQ(pmap2.target_partition, 2);
  EXPECT_EQ(pmap2.num_global_sea_points, 34);
  EXPECT_GT(pmap2.num_local_sea_points, 0);

  // Sum of local sea points across partitions should equal total global sea
  EXPECT_EQ(pmap1.num_local_sea_points + pmap2.num_local_sea_points, 34);

  // Verify consistency of pmap1
  for (int loc_isea = 0; loc_isea < pmap1.num_local_sea_points; ++loc_isea) {
    int glob_isea = pmap1.local_to_global_sea[loc_isea];
    EXPECT_GE(glob_isea, 0);
    EXPECT_LT(glob_isea, 34);
    EXPECT_EQ(pmap1.global_to_local_sea[glob_isea], loc_isea);

    // Verify this point is actually assigned to partition 1
    const Point2D &pt = full_mapping.map_1d_to_2d[glob_isea];
    EXPECT_EQ(partition_map[pt.global_linear_index], 1);
  }

  // Verify that points belonging to partition 2 have -1 in pmap1
  for (int glob_isea = 0; glob_isea < 34; ++glob_isea) {
    const Point2D &pt = full_mapping.map_1d_to_2d[glob_isea];
    if (partition_map[pt.global_linear_index] == 2) {
      EXPECT_EQ(pmap1.global_to_local_sea[glob_isea], -1);
    }
  }
}

TEST(UQGridMappingTest, CreatePartitionMappingReducedGridOverload) {
  const int nx = 8;
  const int ny = 8;
  FullGrid full;
  full.nx = nx;
  full.ny = ny;
  full.land_sea_map.assign(nx * ny, 1);
  full.land_sea_map[0] = 0; // land

  full.longitude.resize(nx);
  for (int x = 0; x < nx; ++x) {
    full.longitude[x] = x * 1.0;
  }
  full.latitude.resize(ny);
  for (int y = 0; y < ny; ++y) {
    full.latitude[y] = y * 1.0;
  }
  full.topobathy.resize(nx * ny, -50.0);
  full.subgrid_obs_x.resize(nx * ny, 1.0);
  full.subgrid_obs_y.resize(nx * ny, 1.0);

  const int num_partitions = 2;
  std::vector<int> partition_map =
      partitionGlobalGrid(nx, ny, full.land_sea_map, num_partitions);

  ReducedGrid red1 = extractOneDomain(full, partition_map, 1, 1);

  GridPointMapping full_mapping =
      createGridPointMapping(nx, ny, full.land_sea_map);

  PartitionMapping pmap_red = createPartitionMapping(full, red1, full_mapping);

  PartitionMapping pmap_direct = createPartitionMapping(
      nx, ny, full.land_sea_map, partition_map, 1, full_mapping);

  // Both methods should identify the same local sea points for partition 1
  EXPECT_EQ(pmap_red.target_partition, 1);
  EXPECT_EQ(pmap_red.num_local_sea_points, pmap_direct.num_local_sea_points);
  EXPECT_EQ(pmap_red.num_global_sea_points, pmap_direct.num_global_sea_points);
  EXPECT_EQ(pmap_red.local_to_global_sea, pmap_direct.local_to_global_sea);
  EXPECT_EQ(pmap_red.global_to_local_sea, pmap_direct.global_to_local_sea);
}

TEST(UQGridMappingTest, CreatePartitionMappingInvalidInputs) {
  std::vector<int> land_sea(16, 1);
  std::vector<int> part_map(16, 1);
  GridPointMapping empty_mapping;

  // Invalid target partition ID
  PartitionMapping pmap_bad =
      createPartitionMapping(4, 4, land_sea, part_map, -1, empty_mapping);
  EXPECT_EQ(pmap_bad.num_local_sea_points, 0);
  EXPECT_TRUE(pmap_bad.local_to_global_sea.empty());

  // Mismatched vector sizes
  std::vector<int> short_vec(5, 1);
  PartitionMapping pmap_short =
      createPartitionMapping(4, 4, short_vec, part_map, 1, empty_mapping);
  EXPECT_EQ(pmap_short.num_local_sea_points, 0);

  // Target partition not present in grid
  PartitionMapping pmap_missing =
      createPartitionMapping(4, 4, land_sea, part_map, 99, empty_mapping);
  EXPECT_EQ(pmap_missing.num_local_sea_points, 0);
  EXPECT_EQ(pmap_missing.num_global_sea_points, 16);
  EXPECT_TRUE(pmap_missing.local_to_global_sea.empty());
  for (int v : pmap_missing.global_to_local_sea) {
    EXPECT_EQ(v, -1);
  }
}
