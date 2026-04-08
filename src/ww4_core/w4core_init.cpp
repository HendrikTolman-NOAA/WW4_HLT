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
 * @date Last Update, 2026-04-08
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_init.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_logfile.hpp"
#include "ww4_utils/ww4_run_config.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <exception>
#include <fstream>
#include <iostream>

namespace ww4_core {

namespace {
ww4_utils::RunConfig globalRunConfig;
std::ofstream logFile;
std::string capturedProgramName;
} // namespace

void w4core_init(const ww4_utils::DateTime &startTime,
                 const std::string &programName) {
  try {
    //
    // 0.  General initialization --------------------------------------------
    // 0.0 Capture program name
    //
    capturedProgramName = programName;

    //
    // 0.1 Load configuration from ww4_run_config.yml file
    //
    auto config = ww4_utils::loadRunConfig("ww4_run_config.yml");
    if (!config) {
      ww4_utils::ww4_std_out::extcde(
          1, std::cerr,
          "Run-time configuration file 'ww4_run_config.yml' not found or could "
          "not be opened.",
          __FILE__, __LINE__);
    }
    globalRunConfig = *config;

    //
    // 0.2 Initialize calendar type in time management service
    //
    ww4_utils::TimeManagement::setCalendarType(globalRunConfig.calendarType);

    //
    // 0.3 Initialize profiling
    //
    ww4_utils::TimeManagement::initializeProfiling();

    //
    // 0.4 Initial standard output if requested
    //
    if (globalRunConfig.produceStdOut) {
      //
      // 0.4.1 Initial standard output
      //
      ww4_utils::ww4_std_out::writeInitialOutput(std::cout,
                                                 capturedProgramName);

      //
      // 0.4.2 Report out run start time
      //
      std::cout << "  Run starts at       : "
                << ww4_utils::TimeManagement::toFormattedString(
                       ww4_utils::TimeManagement::getPresentDateTime())
                << std::endl;

      //
      // 0.4.3 Identify being in initialization routine
      //
      std::cout << "\n  Initialization (w4core_init) starting: "
                << ww4_utils::TimeManagement::toFormattedString(startTime)
                << std::endl;
      //
      // 0.4.4 Report out run time configuration
      //
      ww4_utils::reportRunConfig(globalRunConfig, std::cout);
    }

    //
    // 0.5 Start log file if requested
    //
    if (globalRunConfig.produceLogFile) {
      //
      // 0.5.1 Open log file
      //
      logFile.open("log.ww4");
      //
      // 0.5.2 Initial log file output
      //
      ww4_utils::ww4_logfile::writeInitialOutput(logFile, capturedProgramName);

      //
      // 0.5.3 Report out run start time
      //
      logFile << "  Run starts at       : "
              << ww4_utils::TimeManagement::toFormattedString(
                     ww4_utils::TimeManagement::getPresentDateTime())
              << std::endl;

      //
      // 0.5.4 Identify being in initialization routine
      //
      logFile << "\n  Initialization (w4core_init) starting: "
              << ww4_utils::TimeManagement::toFormattedString(startTime)
              << std::endl;
      //
      // 0.5.5 Report out run time configuration
      //
      ww4_utils::reportRunConfig(globalRunConfig, logFile);
    }

    //
    // 1.  XXXXXXXXX ---------------------------------------------------------
    //

  } catch (const std::exception &e) {
    ww4_utils::ww4_std_out::extcde(1, std::cerr, e.what(), __FILE__, __LINE__);
  } catch (...) {
    ww4_utils::ww4_std_out::extcde(
        1, std::cerr, "Unknown exception in w4core_init", __FILE__, __LINE__);
  }
}

const ww4_utils::RunConfig &getRunConfig() { return globalRunConfig; }

std::ofstream &getLogFileStream() { return logFile; }

const std::string &getProgramName() { return capturedProgramName; }

} // namespace ww4_core
