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
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last update : 2026-06-08
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_init.h"
#include "ww4_utils/time_management.h"
#include "ww4_utils/ww4_input_utils.h"
#include "ww4_utils/ww4_logfile.h"
#include "ww4_utils/ww4_run_config.h"
#include "ww4_utils/ww4_std_out.h"
#include <exception>
#include <fstream>
#include <iostream>

/**
 * @namespace ww4_core
 * @brief Core routines for WAVEWATCH IV.
 */
namespace ww4_core {

namespace {
ww4_utils::RunConfig globalRunConfig;
std::ofstream logFile;
std::string capturedProgramName;
ww4_utils::waveTimeData waveTimeData;
} // namespace

/**
 * @brief Initialization routine for the WAVEWATCH IV core.
 * @details Performs all necessary setup for the wave model core.
 *          Follows the architectural design of the initialization in
 *          ww3_multi.F90 from WAVEWATCH III.
 * @param[in] startTime Simulation start time.
 * @param[in] programName Name of the program to identify in output banners.
 * @param[in] os Output stream for reporting.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-05-01
 */
void w4core_init(const ww4_utils::DateTime &startTime,
                 std::string_view programName, std::ostream &os) {
  try {
    //
    // 1.  General initialization --------------------------------------------
    // 1.0 Capture program name
    //
    capturedProgramName = std::string(programName);

    //
    // 1.1 Load configuration from ww4_run_config.yaml file
    //
    const auto config = ww4_utils::loadRunConfig("ww4_run_config.yaml", os);
    if (!config) {
      ww4_utils::ww4_std_out::extcde(1, os,
                                     "Run-time configuration file "
                                     "'ww4_run_config.yaml' not found or could "
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
    ww4_utils::ww4_input_update(globalRunConfig, os);

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

      //
      // 1.6.6 Write tabular log header
      //
      ww4_utils::ww4_logfile::writeLogTableHeader(logFile);
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
  // 3.1 Initialize model time and time step
  //
  waveTimeData.modelTime = startTime;
  waveTimeData.timeStep = globalRunConfig.timeStep;
  //
  //     End of w4core_init ------------------------------------------------
  //
}

/**
 * @brief Provides access to the loaded run-time configuration.
 * @return A reference to the RunConfig structure.
 */
const ww4_utils::RunConfig &getRunConfig() { return globalRunConfig; }

/**
 * @brief Provides access to the loaded run-time configuration for modification.
 * @return A mutable reference to the RunConfig structure.
 */
ww4_utils::RunConfig &getMutableRunConfig() { return globalRunConfig; }

/**
 * @brief Provides access to the log file stream.
 * @return A reference to the std::ofstream for the log file.
 */
std::ofstream &getLogFileStream() { return logFile; }

/**
 * @brief Provides access to the captured program name.
 * @return A reference to the program name string.
 */
const std::string &getProgramName() { return capturedProgramName; }

/**
 * @brief Provides access to the wave time data.
 * @return A reference to the waveTimeData structure.
 */
const ww4_utils::waveTimeData &getWaveTimeData() { return waveTimeData; }

/**
 * @brief Updates the model time in the wave time data.
 * @param[in] time The new model time.
 */
void updateWaveModelTime(const ww4_utils::DateTime &time) {
  waveTimeData.modelTime = time;
}

/**
 * @brief Updates the input time data for a specific input type.
 * @param[in] type The input type to update.
 * @param[in] data The new input time data.
 */
void updateWaveInputTime(ww4_utils::InputType type,
                         const ww4_utils::intTimeData &data) {
  switch (type) {
  case ww4_utils::InputType::WaterLevels:
    waveTimeData.waterLevels = data;
    break;
  case ww4_utils::InputType::Currents:
    waveTimeData.currents = data;
    break;
  case ww4_utils::InputType::Winds:
    waveTimeData.winds = data;
    break;
  case ww4_utils::InputType::IceConcentrations:
    waveTimeData.iceConcentrations = data;
    break;
  case ww4_utils::InputType::BottomDepth:
    waveTimeData.bottomDepth = data;
    break;
  }
}

/**
 * @brief Resets the internal state of the core module.
 * @details Clears global configuration, program name, and ensures log file
 *          stream is reset. Used during program finalization.
 */
void resetInternalState() noexcept {
  globalRunConfig = ww4_utils::RunConfig();
  capturedProgramName.clear();
  if (logFile.is_open()) {
    logFile.close();
  }
  waveTimeData = ww4_utils::waveTimeData();
  ww4_utils::resetInputData();
}

} // namespace ww4_core
