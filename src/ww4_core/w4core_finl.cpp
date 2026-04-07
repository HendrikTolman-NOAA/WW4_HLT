/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_finl.cpp
 * @brief Implementation of the WAVEWATCH IV finalization routine.
 * @details This file implements the w4core_finl routine.
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

#include "ww4_core/w4core_finl.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_utils/memory_utils.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_logfile.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <exception>
#include <iostream>

namespace ww4_core {

void w4core_finl(const ww4_utils::DateTime &endTime) {
  try {
    //
    // 1.  Capture memory usage and run time ---------------------------------
    //
    const auto memory = ww4_utils::MemoryUtils::captureMemoryUsage();
    const double runTime = ww4_utils::TimeManagement::getProfilingTime();

    //
    // 2.  Final standard output (if requested) ------------------------------
    // 2.1 Initial line
    //
    if (getRunConfig().produceStdOut) {
      std::cout << "\n  Finalization (w4core_finl) starting: "
                << ww4_utils::TimeManagement::toFormattedString(endTime)
                << std::endl;
      //
      // 2.2 Run time and memory usage summary
      //
      ww4_utils::reportRunConfig(getRunConfig(), std::cout);
      //
      ww4_utils::ww4_std_out::writeFinalOutput(std::cout, getProgramName(),
                                               std::nullopt, runTime, memory);
    }

    //
    // 3.  Final log file output (if requested) ------------------------------
    // 3.1 Initial line
    //
    if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
      getLogFileStream() << "\n  Finalization (w4core_finl) starting: "
                         << ww4_utils::TimeManagement::toFormattedString(
                                endTime)
                         << std::endl;

      //
      // 3.2 Run time and memory usage summary
      //
      ww4_utils::reportRunConfig(getRunConfig(), getLogFileStream());
      //
      ww4_utils::ww4_logfile::writeFinalOutput(
          getLogFileStream(), getProgramName(), std::nullopt, runTime, memory);
      //
      // 3.2 Close log file
      //
      getLogFileStream().close();
    }

    //
    // 4.  Release persistent model data -------------------------------------
    // Note: Only globalRunConfig for now
    //
    const_cast<ww4_utils::RunConfig &>(getRunConfig()) = ww4_utils::RunConfig();
  } catch (const std::exception &e) {
    ww4_utils::ww4_std_out::extcde(1, std::cerr, e.what(), __FILE__, __LINE__);
  } catch (...) {
    ww4_utils::ww4_std_out::extcde(
        1, std::cerr, "Unknown exception in w4core_finl", __FILE__, __LINE__);
  }
}

} // namespace ww4_core
