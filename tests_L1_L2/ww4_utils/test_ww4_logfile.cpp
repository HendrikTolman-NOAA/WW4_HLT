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
 * @date Last Update, 2026-04-07
 */

#include "ww4_utils/ww4_logfile.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace ww4_utils::ww4_logfile;
using ww4_utils::MemoryUsage;

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
 * @test Verify the final log output with times and memory.
 */
TEST(LogFileTest, FinalOutputWithMetrics) {
  std::stringstream ss;
  MemoryUsage mem{2000, 1500, 1000, 900};
  writeFinalOutput(ss, "Program shell", 1.23, 4.56, mem);
  std::string output = ss.str();

  EXPECT_NE(output.find("  Initialization time :      1.23 s"),
            std::string::npos);
  EXPECT_NE(output.find("  Elapsed time        :      4.56 s"),
            std::string::npos);
  EXPECT_NE(output.find("  Memory usage:"), std::string::npos);
  EXPECT_NE(
      output.find(
          "    Resident Set Size :        900 kB (Peak: 1000 kB / 0.0082 Gb)"),
      std::string::npos);
  EXPECT_NE(output.find("  End of program "), std::string::npos);
  EXPECT_NE(output.find("WAVEWATCH IV program Program shell"),
            std::string::npos);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
