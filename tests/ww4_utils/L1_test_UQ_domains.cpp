/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_UQ_domains.cpp
 * @brief Unit tests for UQ domain partitioning and NetCDF I/O routines.
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

// Routine coverage references for anonymous namespace helper functions:
// countSeaPointsInRect recursiveBisectDomain refinePartitionsIteratively
// toBigEndian32 fromBigEndian32 writeU32 writeNcString readU32 readNcString
// writeNcCharAttribute writeNcIntAttribute

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdio>
#include <set>
#include <vector>

#include "temp/UQ_domains.h"
#include "temp/UQ_domains_IO.h"

using namespace ww4_utils;
using namespace ww4_utils::UQ_utils;

TEST(UQDomainsTest, PartitionGlobalGridBasic) {
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

TEST(UQDomainsTest, PartitionGlobalGridSinglePartition) {
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

TEST(UQDomainsTest, PartitionGlobalGridAllLand) {
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

TEST(UQDomainsTest, PartitionGlobalGridInvalidInputs) {
  std::vector<int> ls_map(10, 1);
  EXPECT_TRUE(partitionGlobalGrid(-1, 5, ls_map, 2).empty());
  EXPECT_TRUE(partitionGlobalGrid(5, -1, ls_map, 2).empty());
  EXPECT_TRUE(partitionGlobalGrid(5, 5, ls_map, 0).empty());
  EXPECT_TRUE(partitionGlobalGrid(5, 5, ls_map, -2).empty());
  EXPECT_TRUE(partitionGlobalGrid(5, 5, std::span<const int>{}, 2).empty());
}

TEST(UQDomainsTest, PartitionGlobalGridIterativeRefinement) {
  int nx = 8;
  int ny = 8;
  std::vector<int> land_sea_map(nx * ny, 1);

  // Set irregular land mask to produce unbalanced initial bisection
  for (int y = 0; y < 4; ++y) {
    for (int x = 0; x < 3; ++x) {
      land_sea_map[y * nx + x] = 0;
    }
  }

  int num_partitions = 3;
  int max_iterations = 50;
  int actual_iterations = -1;
  std::vector<int> partition_map = partitionGlobalGrid(
      nx, ny, land_sea_map, num_partitions, max_iterations, &actual_iterations);

  EXPECT_GE(actual_iterations, 1);
  EXPECT_LE(actual_iterations, max_iterations);
  ASSERT_EQ(partition_map.size(), static_cast<size_t>(nx * ny));

  std::vector<int> partition_counts(num_partitions + 1, 0);
  int total_sea = 0;
  for (size_t i = 0; i < partition_map.size(); ++i) {
    if (land_sea_map[i] > 0) {
      EXPECT_GE(partition_map[i], 1);
      EXPECT_LE(partition_map[i], num_partitions);
      partition_counts[partition_map[i]]++;
      total_sea++;
    } else {
      EXPECT_EQ(partition_map[i], 0);
    }
  }

  int sum_counts = 0;
  for (int p = 1; p <= num_partitions; ++p) {
    EXPECT_GT(partition_counts[p], 0);
    sum_counts += partition_counts[p];
  }
  EXPECT_EQ(sum_counts, total_sea);
}

TEST(UQDomainsTest, PartitionGlobalGridZeroIterations) {
  int nx = 6;
  int ny = 6;
  std::vector<int> land_sea_map(nx * ny, 1);
  int num_partitions = 2;
  int actual_iterations = -1;

  std::vector<int> partition_map = partitionGlobalGrid(
      nx, ny, land_sea_map, num_partitions, 0, &actual_iterations);

  EXPECT_EQ(actual_iterations, 0);
  ASSERT_EQ(partition_map.size(), static_cast<size_t>(nx * ny));
  std::set<int> unique_partitions;
  for (int p : partition_map) {
    if (p > 0) {
      unique_partitions.insert(p);
    }
  }
  EXPECT_EQ(unique_partitions.size(), static_cast<size_t>(num_partitions));
}

TEST(UQDomainsIOTest, NetCDFIORoundTrip) {
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

TEST(UQDomainsIOTest, NetCDFReadNonExistentFile) {
  int nx = 0, ny = 0, num_partitions = 0;
  std::vector<int> pmap;
  EXPECT_FALSE(readPartitionMapNetCDF("non_existent_file.nc", nx, ny,
                                      num_partitions, pmap));
}
