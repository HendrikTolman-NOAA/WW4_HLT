/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_spatial_grid.cpp
 * @brief Unit tests for spatial grid data structures.
 * @details Uses Google Test to verify FullGrid and LocalGridPartition
 *          construction, accessors, spans, and clear routines.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-21
 * @date Last update : 2026-10-21
 */

#include "ww4_utils/spatial_grid.h"
#include <gtest/gtest.h>

namespace ww4_utils {

TEST(SpatialGridTest, FullGridDefaultsAndClear) {
  FullGrid grid;
  EXPECT_EQ(grid.gridType, GridType::Undefined);
  EXPECT_TRUE(grid.name.empty());
  EXPECT_EQ(grid.nx, 0);
  EXPECT_EQ(grid.ny, 0);
  EXPECT_EQ(grid.numNodes, 0);
  EXPECT_EQ(grid.numElements, 0);
  EXPECT_TRUE(grid.getXCoordinates().empty());
  EXPECT_TRUE(grid.getYCoordinates().empty());
  EXPECT_TRUE(grid.getTopobathy().empty());
  EXPECT_TRUE(grid.getLandMask().empty());
  EXPECT_TRUE(grid.getFaceNodeConnectivity().empty());

  grid.gridType = GridType::Rectangular;
  grid.name = "global_025deg";
  grid.nx = 1440;
  grid.ny = 721;
  grid.numNodes = 1440 * 721;
  grid.xCoordinates = {0.0, 0.25, 0.5};
  grid.yCoordinates = {-90.0, -89.75, -89.5};
  grid.topobathy = {100.0, 50.0, -10.0};
  grid.landMask = {1, 1, 0};
  grid.faceNodeConnectivity = {0, 1, 2};

  EXPECT_EQ(grid.gridType, GridType::Rectangular);
  EXPECT_EQ(grid.name, "global_025deg");
  EXPECT_EQ(grid.getXCoordinates().size(), 3);
  EXPECT_EQ(grid.getYCoordinates().size(), 3);
  EXPECT_EQ(grid.getTopobathy().size(), 3);
  EXPECT_EQ(grid.getLandMask().size(), 3);
  EXPECT_EQ(grid.getFaceNodeConnectivity().size(), 3);

  grid.clear();
  EXPECT_EQ(grid.gridType, GridType::Undefined);
  EXPECT_TRUE(grid.name.empty());
  EXPECT_EQ(grid.nx, 0);
  EXPECT_EQ(grid.ny, 0);
  EXPECT_TRUE(grid.getXCoordinates().empty());
  EXPECT_TRUE(grid.getTopobathy().empty());
}

TEST(SpatialGridTest, LocalGridPartitionDefaultsAndClear) {
  LocalGridPartition part;
  EXPECT_EQ(part.gridType, GridType::Undefined);
  EXPECT_EQ(part.rankId, 0);
  EXPECT_EQ(part.localNx, 0);
  EXPECT_EQ(part.localNy, 0);
  EXPECT_EQ(part.localNumNodes, 0);
  EXPECT_EQ(part.haloSizeX, 0);
  EXPECT_EQ(part.haloSizeY, 0);
  EXPECT_TRUE(part.getXCoordinates().empty());
  EXPECT_TRUE(part.getYCoordinates().empty());
  EXPECT_TRUE(part.getTopobathy().empty());
  EXPECT_TRUE(part.getLandMask().empty());
  EXPECT_TRUE(part.getLocalToGlobalMap().empty());

  part.gridType = GridType::Triangular;
  part.rankId = 2;
  part.localNumNodes = 100;
  part.haloSizeX = 2;
  part.xCoordinates = {10.0, 11.0};
  part.yCoordinates = {20.0, 21.0};
  part.topobathy = {300.0, -50.0};
  part.landMask = {1, 0};
  part.localToGlobalMap = {10, 11};

  EXPECT_EQ(part.gridType, GridType::Triangular);
  EXPECT_EQ(part.rankId, 2);
  EXPECT_EQ(part.getXCoordinates().size(), 2);
  EXPECT_EQ(part.getTopobathy().size(), 2);
  EXPECT_EQ(part.getLocalToGlobalMap().size(), 2);
  EXPECT_EQ(part.getLocalToGlobalMap()[0], 10);

  part.clear();
  EXPECT_EQ(part.gridType, GridType::Undefined);
  EXPECT_EQ(part.rankId, 0);
  EXPECT_TRUE(part.getXCoordinates().empty());
  EXPECT_TRUE(part.getLocalToGlobalMap().empty());
}

} // namespace ww4_utils
