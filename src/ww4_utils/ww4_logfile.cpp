/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_logfile.cpp
 * @brief Implementation of log file output routines.
 * @details This file implements the routines for managing log file output,
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

#include "ww4_utils/ww4_logfile.hpp"
#include <iomanip>

namespace ww4_utils {
namespace ww4_logfile {

void writeInitialOutput(std::ostream &os, const std::string &programName) {
  const std::string mid = "*** WAVEWATCH IV program " + programName + " ***";

  // Generic banner format with 15X (15 spaces) and / (newline)
  const std::string bannerLine = "      " + mid + "      ";
  const std::string separator(bannerLine.length(), '=');

  os << "\n"
     << "               " << bannerLine << "\n"
     << "               " << separator << "\n"
     << std::endl;
}

void writeFinalOutput(std::ostream &os, const std::string &programName,
                      std::optional<double> initTime,
                      std::optional<double> elapsedTotal,
                      std::optional<MemoryUsage> memory) {
  // Matches FORMAT 997, 998, 999
  if (initTime.has_value()) {
    os << "\n  Initialization time :" << std::fixed << std::setprecision(2)
       << std::setw(10) << *initTime << " s\n";
  }
  if (elapsedTotal.has_value()) {
    os << "  Elapsed time        :" << std::fixed << std::setprecision(2)
       << std::setw(10) << *elapsedTotal << " s\n";
  }

  // Optional memory output
  if (memory.has_value()) {
    os << "  Memory usage:\n"
       << "    Peak Virtual Mem  : " << std::setw(10) << memory->vmPeak
       << " kB\n"
       << "    Resident Set Size : " << std::setw(10) << memory->vmRSS
       << " kB (Peak: " << memory->vmHWM << " kB)\n";
  }

  os << "\n  End of program \n"
     << " ========================================\n"
     << "          WAVEWATCH IV program " << programName << " \n"
     << std::endl;
}

} // namespace ww4_logfile
} // namespace ww4_utils
