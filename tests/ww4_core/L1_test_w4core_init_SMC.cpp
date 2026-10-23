/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_w4core_init_SMC.cpp
 * @brief Unit tests for the SMC grid solver initialization module.
 * @details Verifies w4core_init_SMC execution.
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
 * @date Initial, 2026-10-22
 * @date Last update : 2026-10-22
 */

#include "ww4_core/solver_SMC/w4core_init_SMC.h"
#include <gtest/gtest.h>
#include <sstream>

namespace ww4_core {

TEST(W4CoreInitSMCTest, BasicExecution) {
  std::ostringstream ss;
  EXPECT_NO_THROW(w4core_init_SMC(ss));
}

} // namespace ww4_core
