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
 * @date Last update, 2026-04-20
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_finl.hpp"
#include "ww4_core/w4core_hom_input.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_utils/memory_utils.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_logfile.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <exception>
#include <iostream>

namespace ww4_core {

void w4core_finl(const ww4_utils::DateTime &endTime, std::ostream &os) {
  try {
    //
    // 1.  Capture run time --------------------------------------------------
    //
    const double runTime = ww4_utils::TimeManagement::getProfilingTime();

    //
    // 2.  Final standard output (if requested) ------------------------------
    // 2.1 Initial line
    //
    if (getRunConfig().produceStdOut) {
      os << "\n  Finalization (w4core_finl) starting: "
         << ww4_utils::TimeManagement::toFormattedString(endTime) << "\n"
         << std::endl;

      //
      // 2.2 Report out run end time
      //
      os << "  Run ends at "
         << ww4_utils::TimeManagement::toFormattedString(
                ww4_utils::TimeManagement::getPresentDateTime())
         << std::endl;

      //
      // 2.3 Run time summary
      //
      ww4_utils::ww4_std_out::writeFinalOutput(os, getProgramName(),
                                               std::nullopt, runTime);
    }

    //
    // 3.  Final log file output (if requested) ------------------------------
    // 3.1 Initial line
    //
    if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
      getLogFileStream() << "\n  Finalization (w4core_finl) starting: "
                         << ww4_utils::TimeManagement::toFormattedString(
                                endTime)
                         << "\n"
                         << std::endl;

      //
      // 3.2 Report out run end time
      //
      getLogFileStream() << "  Run ends at "
                         << ww4_utils::TimeManagement::toFormattedString(
                                ww4_utils::TimeManagement::getPresentDateTime())
                         << std::endl;

      //
      // 3.3 Run time summary
      //
      ww4_utils::ww4_logfile::writeFinalOutput(
          getLogFileStream(), getProgramName(), std::nullopt, runTime);
      //
      // 3.4 Close log file
      //
      getLogFileStream().close();
    }

    //
    // 4.  Final model time check --------------------------------------------
    //
    if (!getWaveTimeData().modelTime.has_value() ||
        *getWaveTimeData().modelTime != endTime) {
      ww4_utils::ww4_std_out::warnng(
          os, "Model time does not match ending time in w4core_finl", __FILE__,
          __LINE__);
    }

    //
    // 5.  Release persistent model data -------------------------------------
    //
    ww4_utils::TimeManagement::reset();
    ww4_utils::resetMemoryStatusPath();
    resetInputData();
    resetInternalState();
  } catch (const std::exception &e) {
    ww4_utils::ww4_std_out::extcde(1, os, e.what(), __FILE__, __LINE__);
  } catch (...) {
    ww4_utils::ww4_std_out::extcde(1, os, "Unknown exception in w4core_finl",
                                   __FILE__, __LINE__);
  }
}

} // namespace ww4_core
