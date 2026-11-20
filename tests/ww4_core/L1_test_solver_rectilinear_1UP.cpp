/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_solver_rectilinear_1UP.cpp
 * @brief Unit tests for the 1UP solver on the rectilinear grid.
 * @details Verifies name, adding source terms, and solver execution
 *          for SolverRectilinear1UP.
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

#include "ww4_core/grid_rectilinear/solver_rectilinear_1UP.h"
#include "ww4_core/scheme_factory.h"
#include <gtest/gtest.h>
#include <vector>

namespace ww4_core {

TEST(SolverRectilinear1UPTest, BasicProperties) {
  SolverRectilinear1UP solver;
  EXPECT_EQ(solver.getName(), "1UP");
  // Solver-specific initialization is a no-op; grid ingestion is done by the
  // grid (rectilinear).
  EXPECT_NO_THROW(solver.init());
}

TEST(SolverRectilinear1UPTest, SolveAndAddSourceTerm) {
  SolverRectilinear1UP solver;
  std::vector<double> data = {1.0, 2.0, 3.0};

  // Execution without prior addSourceTerm triggers auto-creation of
  // ComputeAllSources (shared SolverBase behavior)
  solver.solve(data);
  EXPECT_GT(data[0], 1.0);

  // Null source terms are ignored
  EXPECT_NO_THROW(solver.addSourceTerm(nullptr));

  // Explicitly add source term and solve again
  auto source = SchemeFactory::createSourceTerm("ST1");
  solver.addSourceTerm(std::move(source));
  EXPECT_NO_THROW(solver.solve(data));
}

} // namespace ww4_core
