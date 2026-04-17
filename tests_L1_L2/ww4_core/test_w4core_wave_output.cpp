/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_w4core_wave_output.cpp
 * @brief Unit tests for w4core_wave output.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Last update, 2026-04-17
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

/**
 * @brief Test fixture for core routine output tests.
 */
class W4CoreWaveOutputTest : public ::testing::Test {
protected:
  void SetUp() override {
    startTime = {19680606, 0.0};
    endTime = {19680606, 3600.0};
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
 * @test Verify w4core_wave reports API output status.
 */
TEST_F(W4CoreWaveOutputTest, ReportsApiOutputStatus) {
  // Test Case 1: API output disabled (default)
  {
    std::ofstream runFile("ww4_run_config.yml");
    runFile << "calendar_type: \"Standard\"\n";
    runFile << "water_levels: none\n";
    runFile << "currents: none\n";
    runFile << "winds: none\n";
    runFile << "ice_concentrations: none\n";
    runFile << "time_step: 3600.0\n";
    runFile << "bottom_depth: none\n";
    runFile << "output_api: no\n";
    runFile.close();

    std::stringstream ss;
    ww4_core::w4core_init(startTime, "test_program", ss);
    ww4_core::w4core_wave(startTime, endTime, ss);

    std::string output = ss.str();
    EXPECT_EQ(output.find("No API output generated"), std::string::npos);
    EXPECT_EQ(output.find("Generating API output"), std::string::npos);
    EXPECT_NE(output.find("No API output"), std::string::npos);
  }

  ww4_core::resetInternalState();

  // Test Case 2: API output enabled
  {
    std::ofstream runFile("ww4_run_config.yml");
    runFile << "calendar_type: \"Standard\"\n";
    runFile << "water_levels: none\n";
    runFile << "currents: none\n";
    runFile << "winds: none\n";
    runFile << "ice_concentrations: none\n";
    runFile << "time_step: 3600.0\n";
    runFile << "bottom_depth: none\n";
    runFile << "output_api: yes\n";
    runFile.close();

    std::stringstream ss;
    ww4_core::w4core_init(startTime, "test_program", ss);
    ww4_core::w4core_wave(startTime, endTime, ss);

    std::string output = ss.str();
    EXPECT_EQ(output.find("Generating API output"), std::string::npos);
    EXPECT_NE(output.find("API output requested"), std::string::npos);
  }
}
