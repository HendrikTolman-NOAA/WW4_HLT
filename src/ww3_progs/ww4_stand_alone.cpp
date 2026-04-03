/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_stand_alone.cpp
 * @brief Stand-alone wave model program.
 * @details This program provides a stand-alone environment for running
 *          the WAVEWATCH IV wave model core.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last Update, 2026-04-03
 * @note This program follows the structure of the stand-alone shell
 *       (ww3_shel.F90) in WAVEWATCH III.
 *       Original author of WW3 stand-alone shell: Hendrik L. Tolman.
 */

#include "ww4_core/ww4_core.hpp"
#include "ww4_utils/memory_utils.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_logfile.hpp"
#include "ww4_utils/ww4_run_config.hpp"
#include "ww4_utils/ww4_stand_alone_config.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <fstream>
#include <iostream>
#include <optional>

/**
 * @brief Main entry point for the ww4_stand_alone program.
 * @return 0 on success, non-zero on failure.
 */
int main() {
  const std::string programName = "ww4_stand_alone";

  // 1. Load run configuration
  ww4_utils::RunConfig runConfig =
      ww4_utils::loadRunConfig("ww4_run_config.yml");

  // 2. Load stand-alone configuration
  std::optional<ww4_utils::StandAloneConfig> saConfig =
      ww4_utils::loadStandAloneConfig("ww4_stand_alone.yml");

  if (!saConfig) {
    std::cerr << "Error: Could not load ww4_stand_alone.yml or invalid content."
              << std::endl;
    return 1;
  }

  // 3. Setup output
  std::ofstream logFile;
  if (runConfig.produceLogFile) {
    logFile.open("log.ww4");
    if (logFile.is_open()) {
      ww4_utils::ww4_logfile::writeInitialOutput(logFile, programName);
    }
  }

  if (runConfig.produceStdOut) {
    ww4_utils::ww4_std_out::writeInitialOutput(std::cout, programName);
  }

  // 4. Start profiling
  ww4_utils::TimeManagement::initializeProfiling();

  // 5. Call core routines
  ww4_core::w4core_init();
  const double initTime = ww4_utils::TimeManagement::getProfilingTime();

  ww4_core::w4core_wave();

  ww4_core::w4core_finl();
  const double totalTime = ww4_utils::TimeManagement::getProfilingTime();

  // 6. Final output
  std::optional<ww4_utils::MemoryUsage> memory =
      ww4_utils::MemoryUtils::captureMemoryUsage();

  if (runConfig.produceStdOut) {
    ww4_utils::ww4_std_out::writeFinalOutput(std::cout, programName, initTime,
                                             totalTime, memory);
  }

  if (logFile.is_open()) {
    ww4_utils::ww4_logfile::writeFinalOutput(logFile, programName, initTime,
                                             totalTime, memory);
    logFile.close();
  }

  return 0;
}
