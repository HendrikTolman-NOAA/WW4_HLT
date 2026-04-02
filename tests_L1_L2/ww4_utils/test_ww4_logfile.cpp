/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_ww4_logfile.cpp
 * @brief Unit tests for log file output routines.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last Update, 2026-04-01
 */

#include "ww4_utils/ww4_logfile.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace ww4_utils::ww4_logfile;

/**
 * @test Verify the initial log banner.
 */
TEST(LogFileTest, InitialOutput) {
  std::stringstream ss;
  writeInitialOutput(ss, "Program shell");
  std::string output = ss.str();

  EXPECT_NE(output.find("*** WAVEWATCH IV program Program shell ***"),
            std::string::npos);
}

/**
 * @test Verify the final log output with times.
 */
TEST(LogFileTest, FinalOutputWithTimes) {
  std::stringstream ss;
  writeFinalOutput(ss, "Program shell", 1.23, 4.56);
  std::string output = ss.str();

  EXPECT_NE(output.find("  Initialization time :      1.23 s"),
            std::string::npos);
  EXPECT_NE(output.find("  Elapsed time        :      4.56 s"),
            std::string::npos);
  EXPECT_NE(output.find("  End of program "), std::string::npos);
  EXPECT_NE(output.find("WAVEWATCH IV program Program shell"), std::string::npos);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
