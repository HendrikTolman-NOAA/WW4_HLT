/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_ww4_std_out.cpp
 * @brief Unit tests for standard screen output routines.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last Update, 2026-04-01
 */

#include "ww4_utils/ww4_std_out.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace ww4_utils::ww4_std_out;
using ww4_utils::MemoryUsage;

/**
 * @test Verify the initial output banner for a Program shell.
 */
TEST(StdOutTest, InitialOutputProgramShell) {
  std::stringstream ss;
  writeInitialOutput(ss, "Program shell");
  std::string output = ss.str();

  // Expected 15 spaces then banner
  EXPECT_NE(
      output.find("      *** WAVEWATCH IV program Program shell ***      "),
      std::string::npos);
  EXPECT_NE(
      output.find("======================================================"),
      std::string::npos);
}

/**
 * @test Verify the final output footer for a Program shell with metrics.
 */
TEST(StdOutTest, FinalOutputProgramShellWithMetrics) {
  std::stringstream ss;
  MemoryUsage mem{1000, 800, 500, 400};
  writeFinalOutput(ss, "Program shell", 1.23, 4.56, mem);
  std::string output = ss.str();

  EXPECT_NE(output.find("  Initialization time :      1.23 s"),
            std::string::npos);
  EXPECT_NE(output.find("  Elapsed time        :      4.56 s"),
            std::string::npos);
  EXPECT_NE(output.find("  Memory usage:"), std::string::npos);
  EXPECT_NE(output.find("    Peak Virtual Mem  :       1000 kB"),
            std::string::npos);
  EXPECT_NE(output.find("  End of program "), std::string::npos);
  EXPECT_NE(output.find("          WAVEWATCH IV program Program shell "),
            std::string::npos);
}

/**
 * @test Verify metrics are omitted when not provided.
 */
TEST(StdOutTest, FinalOutputWithoutMetrics) {
  std::stringstream ss;
  writeFinalOutput(ss, "Program shell");
  std::string output = ss.str();

  EXPECT_EQ(output.find("Initialization time"), std::string::npos);
  EXPECT_EQ(output.find("Memory usage"), std::string::npos);
  EXPECT_NE(output.find("  End of program "), std::string::npos);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
