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
 * @date Last update : 2026-09-24
 */

#include "ww4_core/w4core_init.h"
#include "ww4_core/ww4_source_terms/compute_all_sources.h"
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

namespace ww4_core {

class ComputeAllSourcesL1Test : public ::testing::Test {
protected:
  void SetUp() override { resetInternalState(); }

  void TearDown() override {
    resetInternalState();
    std::remove("ww4_run_config.yaml");
    std::remove("ww4_log.txt");
  }

  void writeYaml(const std::string &inputDiss, const std::string &nlInter,
                 bool sourceTerms = true) {
    std::ofstream runFile("ww4_run_config.yaml");
    runFile << "general:\n";
    runFile << "  time_step: 3600.0\n";
    runFile << "physics:\n";
    runFile << "  solver: uq\n";
    runFile << "  source_terms: " << (sourceTerms ? "yes" : "no") << "\n";
    runFile << "  input_dissipation: " << inputDiss << "\n";
    runFile << "  nonlinear_interactions: " << nlInter << "\n";
    runFile << "forcing:\n";
    runFile << "  water_levels: none\n";
    runFile << "  currents: none\n";
    runFile << "  winds: none\n";
    runFile << "  ice_concentrations: none\n";
    runFile.close();
  }
};

TEST_F(ComputeAllSourcesL1Test, NameCheck) {
  ComputeAllSources sources;
  EXPECT_EQ(sources.getName(), "ComputeAllSources");
}

TEST_F(ComputeAllSourcesL1Test, CalculateST1AndNL1) {
  writeYaml("st1", "nl1");
  ww4_utils::DateTime startTime = {20260101, 0.0};
  w4core_init(startTime, "test_sources", std::cout);

  ComputeAllSources sources;
  sources.init();

  std::vector<double> data = {1.0, 2.5};
  sources.calculate(data);

  // ST1 adds 0.01, NL1 adds 0.001 -> total + 0.011
  EXPECT_NEAR(data[0], 1.011, 1e-9);
  EXPECT_NEAR(data[1], 2.511, 1e-9);
}

TEST_F(ComputeAllSourcesL1Test, CalculateST4AndNL3) {
  writeYaml("st4", "nl3");
  ww4_utils::DateTime startTime = {20260101, 0.0};
  w4core_init(startTime, "test_sources", std::cout);

  ComputeAllSources sources;
  std::vector<double> data = {1.0, 2.5};
  sources.calculate(data); // Implicit init

  // ST4 adds 0.04, NL3 adds 0.003 -> total + 0.043
  EXPECT_NEAR(data[0], 1.043, 1e-9);
  EXPECT_NEAR(data[1], 2.543, 1e-9);
}

TEST_F(ComputeAllSourcesL1Test, CalculateST2ST6AndNL2) {
  writeYaml("st2", "nl2");
  ww4_utils::DateTime startTime = {20260101, 0.0};
  w4core_init(startTime, "test_sources", std::cout);

  ComputeAllSources sources;
  std::vector<double> data = {1.0, 2.5};
  sources.calculate(data);

  // ST2 adds 0.02, NL2 adds 0.002 -> total + 0.022
  EXPECT_NEAR(data[0], 1.022, 1e-9);
  EXPECT_NEAR(data[1], 2.522, 1e-9);

  resetInternalState();
  writeYaml("st6", "nl2");
  w4core_init(startTime, "test_sources_st6", std::cout);

  ComputeAllSources sources2;
  data = {1.0, 2.5};
  sources2.calculate(data);

  // ST6 adds 0.06, NL2 adds 0.002 -> total + 0.062
  EXPECT_NEAR(data[0], 1.062, 1e-9);
  EXPECT_NEAR(data[1], 2.562, 1e-9);
}

TEST_F(ComputeAllSourcesL1Test, CalculateDoNotUse) {
  writeYaml("do_not_use", "do_not_use");
  ww4_utils::DateTime startTime = {20260101, 0.0};
  w4core_init(startTime, "test_sources", std::cout);

  ComputeAllSources sources;
  sources.init();

  std::vector<double> data = {1.0, 2.5};
  sources.calculate(data);

  // No source term modifications
  EXPECT_DOUBLE_EQ(data[0], 1.0);
  EXPECT_DOUBLE_EQ(data[1], 2.5);
}

TEST_F(ComputeAllSourcesL1Test, DisabledSourceTerms) {
  writeYaml("st4", "nl3", false); // source_terms: no
  ww4_utils::DateTime startTime = {20260101, 0.0};
  w4core_init(startTime, "test_sources", std::cout);

  ComputeAllSources sources;
  std::vector<double> data = {1.0, 2.5};
  sources.calculate(data);

  // Disabled source terms -> no change
  EXPECT_DOUBLE_EQ(data[0], 1.0);
  EXPECT_DOUBLE_EQ(data[1], 2.5);
}

} // namespace ww4_core
