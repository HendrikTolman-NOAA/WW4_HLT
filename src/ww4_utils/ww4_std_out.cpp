/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_std_out.cpp
 * @brief Implementation of standard screen output routines.
 * @details This file implements the routines for managing screen output,
 *          matching the formats from WAVEWATCH III.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Aldgisl (AI Persona) (Initial, 2026-04-01)
 * @author Hendrik L. Tolman, Jules (Agentic AI) (Last Update, 2026-04-01)
 * @date Initial, 2026-04-01
 * @date Last Update, 2026-04-01
 * @note Converted from WAVEWATCH III (ww3_shel.F90 and ww3_multi.F90).
 *       Original author: Hendrik L. Tolman.
 */

#include "ww4_utils/ww4_std_out.hpp"

namespace ww4_utils {
namespace ww4_std_out {

void writeInitialOutput(std::ostream &os, const std::string &programName) {
  if (programName == "Program shell") {
    // Matches WW3 ww3_shel.F90 FORMAT 900
    os << "\n"
       << "               "
       << "      *** WAVEWATCH III Program shell ***      \n"
       << "               "
       << "===============================================\n"
       << std::endl;
  } else if (programName == "Multi-grid shell") {
    // Matches WW3 ww3_multi.F90 FORMAT 900
    os << "\n"
       << "               "
       << "     *** WAVEWATCH III Multi-grid shell ***    \n"
       << "               "
       << "=================================================\n"
       << std::endl;
  } else {
    // Generic fallback
    const std::string mid = "*** WAVEWATCH III " + programName + " ***";
    const std::string bannerLine = "      " + mid + "      ";
    const std::string separator(bannerLine.length(), '=');
    os << "\n"
       << "               " << bannerLine << "\n"
       << "               " << separator << "\n"
       << std::endl;
  }
}

void writeFinalOutput(std::ostream &os, const std::string &programName) {
  if (programName == "Program shell") {
    // Matches WW3 ww3_shel.F90 FORMAT 999
    os << "\n  End of program \n"
       << " ====================================\n"
       << "         WAVEWATCH III Program shell \n"
       << std::endl;
  } else if (programName == "Multi-grid shell") {
    // Matches WW3 ww3_multi.F90 FORMAT 999
    os << "\n\n  End of program \n"
       << " ========================================\n"
       << "          WAVEWATCH III Multi-grid shell \n"
       << std::endl;
  } else {
    // Generic fallback
    os << "\n  End of program \n"
       << " ========================================\n"
       << "          WAVEWATCH III " << programName << " \n"
       << std::endl;
  }
}

} // namespace ww4_std_out
} // namespace ww4_utils
