/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_compute_all_sources.cpp
 * @brief Unit tests for physical source term calculations (ComputeAllSources).
 * @details Verifies the behavior and interface compliance of ComputeAllSources.
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
 * @date Initial, 2026-09-15
 * @date Last update : 2026-09-22
 */

#include "ww4_core/ww4_source_terms/compute_all_sources.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace ww4_core {

TEST(ComputeAllSourcesTest, NameCheck) {
  ComputeAllSources sources;
  EXPECT_EQ(sources.getName(), "ComputeAllSources");
}

TEST(ComputeAllSourcesTest, CalculateSources) {
  ComputeAllSources sources;
  std::vector<double> data = {1.0, 2.5, 5.0};
  sources.calculate(data);
  EXPECT_DOUBLE_EQ(data[0], 1.1);
  EXPECT_DOUBLE_EQ(data[1], 2.6);
  EXPECT_DOUBLE_EQ(data[2], 5.1);
}

} // namespace ww4_core
