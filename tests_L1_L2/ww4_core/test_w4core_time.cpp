/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_w4core_time.cpp
 * @brief Unit tests for time passing in core routines.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-06
 * @date Last update, 2026-04-15
 */

#include "ww4_core/w4core_finl.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_core/w4core_wave.hpp"
#include "ww4_utils/time_management.hpp"
#include <gtest/gtest.h>

/**
 * @brief Test fixture for core routine time passing tests.
 */
class W4CoreTimeTest : public ::testing::Test {
protected:
  void SetUp() override {
    startTime = {19680606, 60000.0};
    endTime = {19680606, 180000.0};
  }

  ww4_utils::DateTime startTime;
  ww4_utils::DateTime endTime;
};

/**
 * @test Verify that core routines can be called with DateTime arguments.
 * @details This test ensures that the new signatures are callable and
 *          don't cause any immediate crashes. Since the current
 *          implementation only prints to stdout, we just verify callability.
 */
TEST_F(W4CoreTimeTest, CallCoreRoutinesWithTime) {
  // Create dummy run configuration file
  std::ofstream runFile("ww4_run_config.yml");
  runFile << "calendar_type: \"Standard\"\n";
  runFile << "water_levels: none\n";
  runFile << "currents: none\n";
  runFile << "winds: none\n";
  runFile << "ice_concentrations: none\n";
  runFile << "time_step: 3600.0\n";
  runFile << "bottom_depth: none\n";
  runFile.close();

  EXPECT_NO_THROW(ww4_core::w4core_init(startTime, "test_program"));

  // Verify internal state is set
  EXPECT_EQ(ww4_core::getProgramName(), "test_program");
  EXPECT_EQ(ww4_core::getRunConfig().timeStep, 3600.0);

  EXPECT_NO_THROW(ww4_core::w4core_wave(startTime, endTime));
  EXPECT_NO_THROW(ww4_core::w4core_finl(endTime));

  // Verify internal state is reset after finalization
  EXPECT_EQ(ww4_core::getProgramName(), "");
  EXPECT_EQ(ww4_core::getRunConfig().timeStep, -1.0); // Default value

  std::remove("ww4_run_config.yml");
  std::remove("log.ww4");
}
