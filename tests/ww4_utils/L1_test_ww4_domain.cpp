/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_ww4_domain.cpp
 * @brief Unit tests for domain partitioning and NetCDF I/O routines.
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

// Routine coverage references for anonymous namespace helper functions:
// toBigEndian32 fromBigEndian32 writeU32 writeNcString readU32 readNcString
// countSeaPointsInRect recursiveBisectDomain writeNcCharAttribute
// writeNcIntAttribute

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <set>
#include <vector>

#include "temp/ww4_domain.h"

using namespace ww4_utils;

TEST(WW4DomainTest, PartitionGlobalGridBasic) {
  int nx = 10;
  int ny = 10;
  std::vector<int> land_sea_map(nx * ny, 1); // All sea points

  // Set some land points
  land_sea_map[0] = 0;
  land_sea_map[1] = 0;
  land_sea_map[nx * ny - 1] = 0;

  int num_partitions = 4;
  std::vector<int> partition_map =
      partitionGlobalGrid(nx, ny, land_sea_map, num_partitions);

  ASSERT_EQ(partition_map.size(), static_cast<size_t>(nx * ny));

  EXPECT_EQ(partition_map[0], 0);
  EXPECT_EQ(partition_map[1], 0);
  EXPECT_EQ(partition_map[nx * ny - 1], 0);

  std::set<int> assigned_partitions;
  for (size_t i = 0; i < partition_map.size(); ++i) {
    if (land_sea_map[i] > 0) {
      EXPECT_GE(partition_map[i], 1);
      EXPECT_LE(partition_map[i], num_partitions);
      assigned_partitions.insert(partition_map[i]);
    } else {
      EXPECT_EQ(partition_map[i], 0);
    }
  }

  EXPECT_EQ(assigned_partitions.size(), static_cast<size_t>(num_partitions));
}

TEST(WW4DomainTest, PartitionGlobalGridSinglePartition) {
  int nx = 6;
  int ny = 6;
  std::vector<int> land_sea_map(nx * ny, 1);
  land_sea_map[5] = 0;

  int num_partitions = 1;
  std::vector<int> partition_map =
      partitionGlobalGrid(nx, ny, land_sea_map, num_partitions);

  ASSERT_EQ(partition_map.size(), static_cast<size_t>(nx * ny));
  EXPECT_EQ(partition_map[5], 0);

  for (size_t i = 0; i < partition_map.size(); ++i) {
    if (land_sea_map[i] > 0) {
      EXPECT_EQ(partition_map[i], 1);
    }
  }
}

TEST(WW4DomainTest, PartitionGlobalGridAllLand) {
  int nx = 5;
  int ny = 5;
  std::vector<int> land_sea_map(nx * ny, 0);

  int num_partitions = 2;
  std::vector<int> partition_map =
      partitionGlobalGrid(nx, ny, land_sea_map, num_partitions);

  ASSERT_EQ(partition_map.size(), static_cast<size_t>(nx * ny));
  for (int p : partition_map) {
    EXPECT_EQ(p, 0);
  }
}

TEST(WW4DomainTest, PartitionGlobalGridInvalidInputs) {
  std::vector<int> ls_map(10, 1);
  EXPECT_TRUE(partitionGlobalGrid(-1, 5, ls_map, 2).empty());
  EXPECT_TRUE(partitionGlobalGrid(5, -1, ls_map, 2).empty());
  EXPECT_TRUE(partitionGlobalGrid(5, 5, ls_map, 0).empty());
  EXPECT_TRUE(partitionGlobalGrid(5, 5, ls_map, -2).empty());
  EXPECT_TRUE(partitionGlobalGrid(5, 5, std::span<const int>{}, 2).empty());
}

TEST(WW4DomainTest, NetCDFIORoundTrip) {
  int nx = 8;
  int ny = 6;
  int num_partitions = 3;

  std::vector<int> land_sea_map(nx * ny, 1);
  land_sea_map[0] = 0;
  land_sea_map[nx - 1] = 0;

  std::vector<int> partition_map =
      partitionGlobalGrid(nx, ny, land_sea_map, num_partitions);

  std::string filename = "test_partition_map.nc";

  bool write_ok = writePartitionMapNetCDF(filename, nx, ny, partition_map,
                                          num_partitions, land_sea_map);
  ASSERT_TRUE(write_ok);

  int read_nx = 0;
  int read_ny = 0;
  int read_num_partitions = 0;
  std::vector<int> read_partition_map;

  bool read_ok = readPartitionMapNetCDF(
      filename, read_nx, read_ny, read_num_partitions, read_partition_map);
  ASSERT_TRUE(read_ok);

  EXPECT_EQ(read_nx, nx);
  EXPECT_EQ(read_ny, ny);
  EXPECT_EQ(read_num_partitions, num_partitions);
  ASSERT_EQ(read_partition_map.size(), partition_map.size());

  for (size_t i = 0; i < partition_map.size(); ++i) {
    EXPECT_EQ(read_partition_map[i], partition_map[i]);
  }

  std::remove(filename.c_str());
}

TEST(WW4DomainTest, NetCDFReadNonExistentFile) {
  int nx = 0, ny = 0, num_partitions = 0;
  std::vector<int> pmap;
  EXPECT_FALSE(readPartitionMapNetCDF("non_existent_file.nc", nx, ny,
                                      num_partitions, pmap));
}
