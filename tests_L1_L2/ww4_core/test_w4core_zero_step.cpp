/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_w4core_zero_step.cpp
 * @brief Unit tests for w4core_wave zero time step handling.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#include "ww4_core/w4core_finl.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_core/w4core_wave.hpp"
#include "ww4_utils/time_management.hpp"
#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

class W4CoreZeroStepTest : public ::testing::Test {
protected:
  void SetUp() override {
    startTime = {19680606, 60000.0}; // 06:00:00
    endTime = {19680606, 120000.0};  // 12:00:00
  }

  void TearDown() override {
    ww4_core::resetInternalState();
    std::remove("ww4_run_config.yml");
    std::remove("log.ww4");
  }

  ww4_utils::DateTime startTime;
  ww4_utils::DateTime endTime;
};

/**
 * @test Verify that w4core_wave performs initial output with a zero step.
 */
TEST_F(W4CoreZeroStepTest, PerformsInitialOutputWithZeroStep) {
  std::ofstream runFile("ww4_run_config.yml");
  runFile << "calendar_type: \"Standard\"\n";
  runFile << "water_levels: none\n";
  runFile << "currents: none\n";
  runFile << "winds: none\n";
  runFile << "ice_concentrations: none\n";
  runFile << "time_step: 3600.0\n";
  runFile << "bottom_depth: none\n";
  runFile << "output_fields_requested: yes\n";
  runFile << "output_fields_interval: 3600.0\n";
  runFile << "output_fields_at_first: yes\n";
  runFile.close();

  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test_program", ss);

  ww4_core::w4core_wave(startTime, endTime, ss);

  std::string output = ss.str();

  std::string expectedStep =
      "Step from " + ww4_utils::TimeManagement::toFormattedString(startTime);

  size_t firstStepPos = output.find(expectedStep);
  ASSERT_NE(firstStepPos, std::string::npos);

  size_t outputPos = output.find("Performing fields output", firstStepPos);
  ASSERT_NE(outputPos, std::string::npos);

  size_t secondStepPos = output.find(expectedStep, outputPos);
  ASSERT_NE(secondStepPos, std::string::npos);
}

/**
 * @test Verify that w4core_wave fails with two consecutive zero steps.
 */
TEST_F(W4CoreZeroStepTest, FailsWithTwoConsecutiveZeroSteps) {
  std::ofstream runFile("ww4_run_config.yml");
  runFile << "calendar_type: \"Standard\"\n";
  runFile << "water_levels: none\n";
  runFile << "currents: none\n";
  runFile << "winds: none\n";
  runFile << "ice_concentrations: none\n";
  runFile << "time_step: 0.0\n";
  runFile << "bottom_depth: none\n";
  runFile.close();

  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test_program", ss);

  EXPECT_DEATH(ww4_core::w4core_wave(startTime, endTime, std::cerr),
               "Two consecutive zero time steps detected.");
}
