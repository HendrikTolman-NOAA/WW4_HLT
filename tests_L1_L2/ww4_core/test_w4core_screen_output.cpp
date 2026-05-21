/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_w4core_screen_output.cpp
 * @brief Unit tests for screen output levels in w4core_wave.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Last update : 2026-05-20
 */

#include "ww4_core/w4core_finalize.h"
#include "ww4_core/w4core_init.h"
#include "ww4_core/w4core_wave.h"
#include "ww4_utils/time_management.h"
#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

class W4CoreScreenOutputTest : public ::testing::Test {
protected:
  void SetUp() override {
    startTime = {19680606, 0.0};
    endTime = {19680606, 20000.0}; // 2 hours (HHMMSS)
  }

  void TearDown() override {
    ww4_core::resetInternalState();
    std::remove("ww4_run_config.yaml");
    std::remove("log.ww4");
  }

  void createConfig(const std::string &screenLevel, bool outputRequested) {
    std::ofstream runFile("ww4_run_config.yaml");
    runFile << "calendar_type: \"Standard\"\n";
    runFile << "water_levels: homogeneous\n";
    runFile << "- 19680606 000000 1.0\n";
    runFile << "- 19680606 010000 2.0\n"; // Update at 1 hour
    runFile << "currents: none\n";
    runFile << "winds: none\n";
    runFile << "ice_concentrations: none\n";
    runFile << "time_step: 1800.0\n"; // 30 min
    runFile << "bottom_depth: none\n";
    runFile << "screen_output_level: " << screenLevel << "\n";
    if (outputRequested) {
      runFile << "output_fields_requested: yes\n";
      runFile << "output_fields_interval: 7200.0\n"; // at 2 hours
      runFile << "output_fields_at_first: no\n";
    }
    runFile.close();
  }

  ww4_utils::DateTime startTime;
  ww4_utils::DateTime endTime;
};

TEST_F(W4CoreScreenOutputTest, FullOutput) {
  createConfig("full", false);
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test_program", ss);
  ww4_core::w4core_wave(startTime, endTime, ss);

  std::string output = ss.str();

  // Should have header for every step (4 steps: 00:00, 00:30, 01:00, 01:30)
  // Step 1: 00:00:00, actualTimeStep = 1800 -> modelTime 00:30:00
  // Step 2: 00:30:00, actualTimeStep = 1800 -> modelTime 01:00:00
  // Step 3: 01:00:00, actualTimeStep = 1800 -> modelTime 01:30:00
  // Step 4: 01:30:00, actualTimeStep = 1800 -> modelTime 02:00:00

  size_t count = 0;
  size_t pos = output.find("Computation step starting");
  while (pos != std::string::npos) {
    count++;
    pos = output.find("Computation step starting", pos + 1);
  }
  EXPECT_EQ(count, 4);
  EXPECT_NE(output.find("Updating water levels"), std::string::npos);
}

TEST_F(W4CoreScreenOutputTest, NoneOutput) {
  createConfig("none", true);
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test_program", ss);
  ww4_core::w4core_wave(startTime, endTime, ss);

  std::string output = ss.str();
  // Should NOT have headers or update/output messages in the loop
  EXPECT_EQ(output.find("Computation step starting"), std::string::npos);
  EXPECT_EQ(output.find("Updating water levels"), std::string::npos);
  EXPECT_EQ(output.find("Performing fields output"), std::string::npos);
}

TEST_F(W4CoreScreenOutputTest, SummaryOutput) {
  createConfig("summary", true);
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test_program", ss);
  ww4_core::w4core_wave(startTime, endTime, ss);

  std::string output = ss.str();

  // Steps:
  // 00:00:00: Updating water levels (interpolation from 00:00 to 01:00) ->
  // PRINT 00:30:00: No update, no output -> SKIP 01:00:00: Updating water
  // levels (interpolation from 01:00 to run end) -> PRINT 01:30:00: No update,
  // but at 02:00:00 (end of step) we have output -> PRINT

  // Note: Interpolation info is printed during updateAllInputs
  // At 00:00:00, it updates.
  // At 00:30:00, it does NOT update (interpolation window same).
  // At 01:00:00, it updates.
  // At 01:30:00, it does NOT update, BUT modelTime becomes 02:00:00, and output
  // is performed.

  size_t count = 0;
  size_t pos = output.find("Computation step starting");
  while (pos != std::string::npos) {
    count++;
    pos = output.find("Computation step starting", pos + 1);
  }
  EXPECT_EQ(count, 3);

  EXPECT_NE(output.find("Updating water levels"), std::string::npos);
  EXPECT_NE(output.find("Performing fields output"), std::string::npos);
}

TEST_F(W4CoreScreenOutputTest, LogFileTabularOutput) {
  createConfig("none", true);
  std::stringstream ss;
  ww4_core::w4core_init(startTime, "test_program", ss);
  ww4_core::w4core_wave(startTime, endTime, ss);

  std::ifstream logFile("log.ww4");
  std::stringstream logContent;
  logContent << logFile.rdbuf();
  std::string logStr = logContent.str();

  // Should have tabular header
  EXPECT_NE(logStr.find("Inputs        |         Outputs"), std::string::npos);
  EXPECT_NE(logStr.find("WLV CUR WND ICE DPT | FLD PNT NST TRK RST API"),
            std::string::npos);

  // Should have 3 lines of data (00:00:00, 01:00:00, 02:00:00)
  // We check for 'X' in the columns
  size_t count = 0;
  size_t pos = logStr.find("X");
  while (pos != std::string::npos) {
    // Only count if it looks like a table line (starts with a date-like string)
    // For simplicity, just count 'X's if we know the expected number of events
    // 00:00:00: Water level 'X'
    // 01:00:00: Water level 'X'
    // 02:00:00: Fields 'X'
    count++;
    pos = logStr.find("X", pos + 1);
  }
  EXPECT_EQ(count, 3);
}
