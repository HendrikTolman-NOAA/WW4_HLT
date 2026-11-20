/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_grid_rectilinear.cpp
 * @brief Unit tests for the rectilinear grid type.
 * @details Verifies grid name, available solvers, the per-grid solver
 *          factory, and grid initialization for GridRectilinear.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 *
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 *
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Kiro (Agentic AI)
 * @date Initial, 2026-11-20
 * @date Last update : 2026-11-20
 */

#include "ww4_core/grid_rectilinear/grid_rectilinear.h"
#include "ww4_core/grid_rectilinear/utils_UQ_IO/test_ugrid_helper.h"
#include "ww4_core/scheme_factory.h"
#include "ww4_core/w4core_init.h"
#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace ww4_core {

TEST(GridRectilinearTest, NameAndSolverNames) {
  GridRectilinear grid;
  EXPECT_EQ(grid.getName(), "rectilinear");
  const std::vector<std::string_view> expected = {"1UP", "UQ"};
  ASSERT_EQ(grid.getSolverNames().size(), expected.size());
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_EQ(grid.getSolverNames()[i], expected[i]);
    EXPECT_TRUE(grid.hasSolver(expected[i]));
  }
  EXPECT_FALSE(grid.hasSolver("UNKNOWN"));
}

TEST(GridRectilinearTest, CreateSolvers) {
  GridRectilinear grid;
  for (const auto name : grid.getSolverNames()) {
    auto solver = grid.createSolver(name);
    ASSERT_NE(solver, nullptr);
    EXPECT_EQ(solver->getName(), name);
  }
  EXPECT_THROW((void)grid.createSolver("UNKNOWN"), std::invalid_argument);
  // Solvers of other grids are not available on this grid
  EXPECT_THROW((void)grid.createSolver("implicit"), std::invalid_argument);
  EXPECT_THROW((void)grid.createSolver("explicit"), std::invalid_argument);
  EXPECT_THROW((void)grid.createSolver("UKMET"), std::invalid_argument);
}

TEST(GridRectilinearTest, InitReadsGrid) {
  resetInternalState();
  getMutableRunConfig().gridFile = "ww4_grid_rect_test.nc";
  ww4::test_helpers::createMinimalUgridFile("ww4_grid_rect_test.nc", 10, 10);

  GridRectilinear grid;
  std::ostringstream ss;
  EXPECT_NO_THROW(grid.init(ss));
  EXPECT_NE(ss.str().find("Processing the grid:"), std::string::npos);

  std::remove("ww4_grid_rect_test.nc");
  resetInternalState();
}

} // namespace ww4_core
