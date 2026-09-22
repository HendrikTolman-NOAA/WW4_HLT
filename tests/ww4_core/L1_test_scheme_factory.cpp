/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_scheme_factory.cpp
 * @brief Unit tests for the SchemeFactory class.
 * @details Verifies the instantiation of numerical solvers and source term
 *          schemes using SchemeFactory.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 *
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 *
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-06-24
 * @date Last update : 2026-09-22
 */

#include "ww4_core/scheme_factory.hpp"
#include <gtest/gtest.h>

namespace ww4_core {

TEST(SchemeFactoryTest, CreateSolverUQ) {
  auto solver = SchemeFactory::createSolver("UQ");
  ASSERT_NE(solver, nullptr);
  EXPECT_EQ(solver->getName(), "UQ");

  auto solverGrid = SchemeFactory::createSolver("RectangularGrid");
  ASSERT_NE(solverGrid, nullptr);
  EXPECT_EQ(solverGrid->getName(), "UQ");
}

TEST(SchemeFactoryTest, CreateSolverTriangular) {
  auto solver = SchemeFactory::createSolver("Triangular");
  ASSERT_NE(solver, nullptr);
  EXPECT_EQ(solver->getName(), "Triangular");

  auto solverGrid = SchemeFactory::createSolver("TriangularGrid");
  ASSERT_NE(solverGrid, nullptr);
  EXPECT_EQ(solverGrid->getName(), "Triangular");
}

TEST(SchemeFactoryTest, CreateSolverSMC) {
  auto solver = SchemeFactory::createSolver("SMC");
  ASSERT_NE(solver, nullptr);
  EXPECT_EQ(solver->getName(), "SMC");

  auto solverGrid = SchemeFactory::createSolver("SMCGrid");
  ASSERT_NE(solverGrid, nullptr);
  EXPECT_EQ(solverGrid->getName(), "SMC");
}

TEST(SchemeFactoryTest, CreateSourceComputeAllSources) {
  auto scheme = SchemeFactory::createSourceTerm("compute_all_sources");
  ASSERT_NE(scheme, nullptr);
  EXPECT_EQ(scheme->getName(), "ComputeAllSources");
}

TEST(SchemeFactoryTest, CreateUnknownSolver) {
  EXPECT_THROW(SchemeFactory::createSolver("UNKNOWN"), std::invalid_argument);
}

TEST(SchemeFactoryTest, CreateUnknownSource) {
  EXPECT_THROW(SchemeFactory::createSourceTerm("UNKNOWN"),
               std::invalid_argument);
}

} // namespace ww4_core
