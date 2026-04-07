/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_init.cpp
 * @brief Implementation of the WAVEWATCH IV initialization routine.
 * @details This file implements the w4core_init routine.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last Update, 2026-04-07
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_init.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_logfile.hpp"
#include "ww4_utils/ww4_run_config.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <fstream>
#include <iostream>

namespace ww4_core {

namespace {
ww4_utils::RunConfig globalRunConfig;
std::ofstream logFile;
} // namespace

void w4core_init(const ww4_utils::DateTime &startTime) {
  //
  // 0.  General initialization ----------------------------------------------
  // 0.1 Load configuration from ww4_run_config.yml file
  //
  globalRunConfig = ww4_utils::loadRunConfig("ww4_run_config.yml");

  //
  // 0.2 Initial standard output if requested
  //
  if (globalRunConfig.produceStdOut) {
    //
    // 0.2.1 Initial standard output
    //
    ww4_utils::ww4_std_out::writeInitialOutput(std::cout, "ww4_stand_alone");
    //
    // 0.2.2 Identify being in initialization routine
    //
    std::cout << "          * Initialization (w4core_init) starting: "
              << ww4_utils::TimeManagement::toFormattedString(startTime)
              << std::endl;
    //
    // 0.2.3 Report out run time configuration
    //
    ww4_utils::reportRunConfig(globalRunConfig, std::cout);
  }

  //
  // 0.3 Start log file if requested
  //
  if (globalRunConfig.produceLogFile) {
    //
    // 0.3.1 Open log file
    //
    logFile.open("log.ww4");
    //
    // 0.3.2 Initial log file output
    //
    ww4_utils::ww4_logfile::writeInitialOutput(logFile, "ww4_stand_alone");
    //
    // 0.3.3 Identify being in initialization routine
    //
    logFile << "          * Initialization (w4core_init) starting: "
            << ww4_utils::TimeManagement::toFormattedString(startTime)
            << std::endl;
    //
    // 0.3.4 Report out run time configuration
    //
    ww4_utils::reportRunConfig(globalRunConfig, logFile);
  }

  //
  // 0.4 Initialize profiling
  //
  ww4_utils::TimeManagement::initializeProfiling();

  //
  // 1.  XXXXXXXXX -----------------------------------------------------------
  //
}

const ww4_utils::RunConfig &getRunConfig() { return globalRunConfig; }

std::ofstream &getLogFileStream() { return logFile; }

} // namespace ww4_core
