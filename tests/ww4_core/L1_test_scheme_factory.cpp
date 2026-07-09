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
 * @date Last update : 2026-06-24
 */

#include "ww4_core/scheme_factory.hpp"
#include <gtest/gtest.h>

namespace ww4_core {

TEST(SchemeFactoryTest, CreateSolverPR3) {
  auto solver = SchemeFactory::createSolver("PR3");
  ASSERT_NE(solver, nullptr);
  EXPECT_EQ(solver->getName(), "PR3");
}

TEST(SchemeFactoryTest, CreateSourceST4) {
  auto scheme = SchemeFactory::createSourceTerm("ST4");
  ASSERT_NE(scheme, nullptr);
  EXPECT_EQ(scheme->getName(), "ST4");
}

TEST(SchemeFactoryTest, CreateUnknownSolver) {
  EXPECT_THROW(SchemeFactory::createSolver("UNKNOWN"), std::invalid_argument);
}

TEST(SchemeFactoryTest, CreateUnknownSource) {
  EXPECT_THROW(SchemeFactory::createSourceTerm("UNKNOWN"),
               std::invalid_argument);
}

} // namespace ww4_core
