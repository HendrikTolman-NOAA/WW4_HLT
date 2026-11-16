/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_solver_triangular_init.cpp
 * @brief Unit tests for the triangular grid solver initialization module.
 * @details Verifies solver_triangular_init execution.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 *
 * The National Weather Service (NWS) often uses Generative SI (GenSI, formally
 * GenAI) tools for code development and refactoring. Whenever such tools are
 * used, NWS requires disclosure and a full human review of changes before they
 * are applied to NWS repositories.
 *
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI/SI)
 * @date Initial: 2026-10-23
 * @date Last update: 2026-11-16
 */

#include "ww4_core/solver_triangular/solver_triangular_init.h"
#include <gtest/gtest.h>
#include <sstream>

namespace ww4_core {

TEST(SolverTriangularInitTest, AbortsOnBottomGrid) {
  EXPECT_EXIT(solver_triangular_init(std::cerr), ::testing::ExitedWithCode(1),
              "WW4 ERROR: Triangular unstructured grid reader not yet "
              "implemented when reading bottom grid.");
}

} // namespace ww4_core
