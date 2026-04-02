/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_std_out.cpp
 * @brief Implementation of standard screen output routines.
 * @details This file implements the routines for managing screen output,
 *          matching the formats from WAVEWATCH III, updated for WAVEWATCH IV.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last Update, 2026-04-01
 * @note Converted from WAVEWATCH III (ww3_shel.F90 and ww3_multi.F90).
 *       Original author: Hendrik L. Tolman.
 */

#include "ww4_utils/ww4_std_out.hpp"

namespace ww4_utils {
namespace ww4_std_out {

void writeInitialOutput(std::ostream &os, const std::string &programName) {
  // Matches WW3 aesthetics, updated for WW4
  const std::string mid = "*** WAVEWATCH IV program " + programName + " ***";

  // Generic banner format with 15X (15 spaces) and / (newline)
  const std::string bannerLine = "      " + mid + "      ";
  const std::string separator(bannerLine.length(), '=');

  os << "\n"
     << "               " << bannerLine << "\n"
     << "               " << separator << "\n"
     << std::endl;
}

void writeFinalOutput(std::ostream &os, const std::string &programName) {
  // Matches WW3 aesthetics, updated for WW4
  os << "\n  End of program \n"
     << " ========================================\n"
     << "          WAVEWATCH IV program " << programName << " \n"
     << std::endl;
}

} // namespace ww4_std_out
} // namespace ww4_utils
