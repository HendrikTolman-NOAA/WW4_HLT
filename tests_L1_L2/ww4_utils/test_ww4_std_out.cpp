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
 * @author Main Author(s): Aldgisl (AI Persona)
 * @author Contributors: Hendrik L. Tolman, Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last Update, 2026-04-01
 */

#include "ww4_utils/ww4_std_out.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace ww4_utils::ww4_std_out;

/**
 * @test Verify the initial output banner for the Program shell.
 */
TEST(StdOutTest, InitialOutputProgramShell) {
  std::stringstream ss;
  writeInitialOutput(ss, "Program shell");
  std::string output = ss.str();

  // Expected 15 spaces then banner
  EXPECT_NE(output.find("      *** WAVEWATCH III Program shell ***      "),
            std::string::npos);
  EXPECT_NE(output.find("==============================================="),
            std::string::npos);
}

/**
 * @test Verify the initial output banner for the Multi-grid shell.
 */
TEST(StdOutTest, InitialOutputMultiGrid) {
  std::stringstream ss;
  writeInitialOutput(ss, "Multi-grid shell");
  std::string output = ss.str();

  EXPECT_NE(output.find("     *** WAVEWATCH III Multi-grid shell ***    "),
            std::string::npos);
  EXPECT_NE(output.find("================================================="),
            std::string::npos);
}

/**
 * @test Verify the final output footer for the Program shell.
 */
TEST(StdOutTest, FinalOutputProgramShell) {
  std::stringstream ss;
  writeFinalOutput(ss, "Program shell");
  std::string output = ss.str();

  EXPECT_NE(output.find("  End of program "), std::string::npos);
  EXPECT_NE(output.find("         WAVEWATCH III Program shell "),
            std::string::npos);
  EXPECT_NE(output.find(" ===================================="),
            std::string::npos);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
