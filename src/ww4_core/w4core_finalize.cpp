/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_finalize.cpp
 * @brief Implementation of the WAVEWATCH IV finalization routine.
 * @details This file implements the w4core_finalize routine.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring. Whenever GenAI is used, NWS requires a full human review of code before it is added to its repositories
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last update : 2026-05-21
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_finalize.h"
#include "ww4_core/w4core_init.h"
#include "ww4_utils/memory_utils.h"
#include "ww4_utils/time_management.h"
#include "ww4_utils/ww4_input_utils.h"
#include "ww4_utils/ww4_logfile.h"
#include "ww4_utils/ww4_std_out.h"
#include <exception>
#include <iostream>

/**
 * @namespace ww4_core
 * @brief Core routines for WAVEWATCH IV.
 */
namespace ww4_core {

/**
 * @brief Finalization routine for the WAVEWATCH IV core.
 * @details Performs all necessary cleanup and final reporting for the
 *          wave model core. Follows the architectural design of the
 *          finalization in ww3_multi.F90 from WAVEWATCH III.
 * @param[in] endTime Simulation end time.
 * @param[in] os Output stream for reporting.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-05-21
 */
void w4core_finalize(const ww4_utils::DateTime &endTime, std::ostream &os) {
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
      os << "\n  Finalization (w4core_finalize) starting: "
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
    //
    if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
      //
      // 3.1 Write tabular log footer
      //
      ww4_utils::ww4_logfile::writeLogTableFooter(getLogFileStream());

      //
      // 3.2 Initial line
      //
      getLogFileStream() << "\n  Finalization (w4core_finalize) starting: "
                         << ww4_utils::TimeManagement::toFormattedString(
                                endTime)
                         << "\n"
                         << std::endl;

      //
      // 3.3 Report out run end time
      //
      getLogFileStream() << "  Run ends at "
                         << ww4_utils::TimeManagement::toFormattedString(
                                ww4_utils::TimeManagement::getPresentDateTime())
                         << std::endl;

      //
      // 3.4 Run time summary
      //
      ww4_utils::ww4_logfile::writeFinalOutput(
          getLogFileStream(), getProgramName(), std::nullopt, runTime);
      //
      // 3.5 Close log file
      //
      getLogFileStream().close();
    }

    //
    // 4.  Release persistent model data -------------------------------------
    //
    // 4.1 Check model time versus end time
    //
    if (getWaveTimeData().modelTime.has_value() &&
        *getWaveTimeData().modelTime != endTime) {
      ww4_utils::ww4_std_out::warnng(os, "Model time does not match end time.",
                                     __FILE__, __LINE__);
    }

    ww4_utils::TimeManagement::reset();
    ww4_utils::resetMemoryStatusPath();
    ww4_utils::resetInputData();
    resetInternalState();
  } catch (const std::exception &e) {
    ww4_utils::ww4_std_out::extcde(1, os, e.what(), __FILE__, __LINE__);
  } catch (...) {
    ww4_utils::ww4_std_out::extcde(
        1, os, "Unknown exception in w4core_finalize", __FILE__, __LINE__);
  }
}

} // namespace ww4_core
