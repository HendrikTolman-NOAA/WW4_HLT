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
 * @date Last update, 2026-04-20
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_init.hpp"
#include "ww4_core/w4core_hom_input.hpp"
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
                 std::string_view programName, std::ostream &os) {
  try {
    //
    // 1.  General initialization --------------------------------------------
    // 1.0 Capture program name
    //
    capturedProgramName = std::string(programName);

    //
    // 1.1 Load configuration from ww4_run_config.yml file
    //
    const auto config = ww4_utils::loadRunConfig("ww4_run_config.yml", os);
    if (!config) {
      ww4_utils::ww4_std_out::extcde(
          1, os,
          "Run-time configuration file 'ww4_run_config.yml' not found or could "
          "not be opened.",
          __FILE__, __LINE__);
    }
    globalRunConfig = *config;

    //
    // 1.2 Initialize calendar type in time management service
    //
    ww4_utils::TimeManagement::setCalendarType(globalRunConfig.calendarType);

    //
    // 1.3 Initialize profiling
    //
    ww4_utils::TimeManagement::initializeProfiling();

    //
    // 1.4 Process homogeneous input data silently
    //
    w4core_hom_input(os);

    //
    // 1.5 Initial standard output if requested
    //
    if (globalRunConfig.produceStdOut) {
      //
      // 1.5.1 Initial standard output
      //
      ww4_utils::ww4_std_out::writeInitialOutput(os, capturedProgramName);

      //
      // 1.5.2 Report out run start time
      //
      os << "  Run starts at "
         << ww4_utils::TimeManagement::toFormattedString(
                ww4_utils::TimeManagement::getPresentDateTime())
         << std::endl;

      //
      // 1.5.3 Identify being in initialization routine
      //
      os << "\n  Initialization (w4core_init) starting: "
         << ww4_utils::TimeManagement::toFormattedString(startTime)
         << std::endl;
      //
      // 1.5.4 Report out run time configuration
      //
      ww4_utils::reportRunConfig(globalRunConfig, os);
    }

    //
    // 1.6 Start log file if requested
    //
    if (globalRunConfig.produceLogFile) {
      //
      // 1.6.1 Open log file
      //
      logFile.open("log.ww4");
      //
      // 1.6.2 Initial log file output
      //
      ww4_utils::ww4_logfile::writeInitialOutput(logFile, capturedProgramName);

      //
      // 1.6.3 Report out run start time
      //
      logFile << "  Run starts at "
              << ww4_utils::TimeManagement::toFormattedString(
                     ww4_utils::TimeManagement::getPresentDateTime())
              << std::endl;

      //
      // 1.6.4 Identify being in initialization routine
      //
      logFile << "\n  Initialization (w4core_init) starting: "
              << ww4_utils::TimeManagement::toFormattedString(startTime)
              << std::endl;
      //
      // 1.6.5 Report out run time configuration
      //
      ww4_utils::reportRunConfig(globalRunConfig, logFile);
    }

  } catch (const std::exception &e) {
    ww4_utils::ww4_std_out::extcde(1, os, e.what(), __FILE__, __LINE__);
  } catch (...) {
    ww4_utils::ww4_std_out::extcde(1, os, "Unknown exception in w4core_init",
                                   __FILE__, __LINE__);
  }
    //
    // 2.  Set up data strucures ---------------------------------------------
    // 2.1 Set up the global and local (domain) grids
    //
    // 2.2  Set up the spectral data structures
    //
    //
    // 3.  Data initialization -----------------------------------------------
    //     This only needs to be the intial conditions, Input and output are
    //     initialized as part of their update procedures in w4core_wave
    //
    //
    //     End of w4core_init ------------------------------------------------
    //
}

const ww4_utils::RunConfig &getRunConfig() { return globalRunConfig; }

std::ofstream &getLogFileStream() { return logFile; }

const std::string &getProgramName() { return capturedProgramName; }

void resetInternalState() noexcept {
  globalRunConfig = ww4_utils::RunConfig();
  capturedProgramName.clear();
  if (logFile.is_open()) {
    logFile.close();
  }
  resetInputData();
}

} // namespace ww4_core
