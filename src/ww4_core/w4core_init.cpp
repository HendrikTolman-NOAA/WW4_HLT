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
 * @date Last Update, 2026-04-06
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
  // Load configuration
  globalRunConfig = ww4_utils::loadRunConfig("ww4_run_config.yml");

  //
  // Initialize profiling
  ww4_utils::TimeManagement::initializeProfiling();

  //
  // Open log file if requested
  if (globalRunConfig.produceLogFile) {
    logFile.open("log.ww4");
  }

  //
  // Initial standard output
  if (globalRunConfig.produceStdOut) {
    ww4_utils::ww4_std_out::writeInitialOutput(std::cout, "Multi-grid shell");
    std::cout << "          * Initialization (w4core_init) starting: "
              << ww4_utils::TimeManagement::toFormattedString(startTime)
              << std::endl;
    //
    reportRunConfig(globalRunConfig, std::cout);
  }

  //
  // Initial log file output
  if (globalRunConfig.produceLogFile && logFile.is_open()) {
    ww4_utils::ww4_logfile::writeInitialOutput(logFile, "Multi-grid shell");
    logFile << "          * Initialization (w4core_init) starting: "
            << ww4_utils::TimeManagement::toFormattedString(startTime)
            << std::endl;
    //
    reportRunConfig(globalRunConfig, logFile);
  }
}

const ww4_utils::RunConfig &getRunConfig() { return globalRunConfig; }

std::ofstream &getLogFileStream() { return logFile; }

void reportRunConfig(const ww4_utils::RunConfig &config, std::ostream &os) {
  os << "          Configuration settings :" << std::endl;

  std::string calType = "Standard";
  if (config.calendarType == ww4_utils::TimeManagement::CalendarType::NoLeap) {
    calType = "NoLeap";
  } else if (config.calendarType ==
             ww4_utils::TimeManagement::CalendarType::ThreeSixtyDay) {
    calType = "ThreeSixtyDay";
  }

  os << "            Calendar type      : " << calType << std::endl;
  os << "            Screen output      : "
     << (config.produceStdOut ? "yes" : "no") << std::endl;
  os << "            Log file           : "
     << (config.produceLogFile ? "yes" : "no") << std::endl;
  os << std::endl;
}

} // namespace ww4_core
