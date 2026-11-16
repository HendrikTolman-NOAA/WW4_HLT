/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_ww4_BT4.cpp
 * @brief Unit tests for BT4 bottom friction source term.
 * @details Verifies the behavior and interface compliance of SourceTermBT4.
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
 * @date Initial: 2026-09-24
 * @date Last update: 2026-11-16
 */

#include "ww4_core/source_terms/ww4_BT4/ww4_BT4.h"
#include <gtest/gtest.h>
#include <vector>

namespace ww4_core {

TEST(BT4Test, BasicPropertiesAndCalculate) {
  SourceTermBT4 source;
  EXPECT_EQ(source.getName(), "BT4");

  std::vector<double> data = {1.0, 2.0};
  source.calculate(data);
  EXPECT_DOUBLE_EQ(data[0], 0.996);
  EXPECT_DOUBLE_EQ(data[1], 1.996);
}

} // namespace ww4_core
