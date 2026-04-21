/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_wave.cpp
 * @brief Implementation of the WAVEWATCH IV time stepping routine.
 * @details This file implements the w4core_wave routine.
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

#include "ww4_core/w4core_wave.hpp"
#include "ww4_core/w4core_hom_input.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

namespace ww4_core {

void w4core_wave(const ww4_utils::DateTime &startTime,
                 const ww4_utils::DateTime &endTime, std::ostream &os) {
  try {
    //
    // 1.  General initialization  -------------------------------------------
    // 1.1 Output to standard output (if requested)
    //
    if (getRunConfig().produceStdOut) {
      os << "\n  Time stepping (w4core_wave) from: "
         << ww4_utils::TimeManagement::toFormattedString(startTime)
         << " to: " << ww4_utils::TimeManagement::toFormattedString(endTime)
         << std::endl;
    }
    //
    // 1.2 Output to  log file (if requested)  likely to be temporarily as the
    //     eventually the log file will be to consice for this output
    //
    if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
      getLogFileStream()
          << "\n  Time stepping (w4core_wave) from: "
          << ww4_utils::TimeManagement::toFormattedString(startTime)
          << " to: " << ww4_utils::TimeManagement::toFormattedString(endTime)
          << std::endl;
    }
    //
    // 1.3 Check consistency of starting and ending times
    // 1.3.1 Starting versus ending time
    //
    if (ww4_utils::TimeManagement::differenceInSeconds(startTime, endTime) <
        0.0) {
      ww4_utils::ww4_std_out::extcde(1, os, "End time before start time.",
                                     __FILE__, __LINE__);
    }
    //
    // 1.3.2 Starting versus model time
    //
    if (!getWaveTimeData().modelTime.has_value()) {
      ww4_utils::ww4_std_out::extcde(1, os, "Model time not initialized.",
                                     __FILE__, __LINE__);
    }

    if (*getWaveTimeData().modelTime != startTime) {
      ww4_utils::ww4_std_out::extcde(
          1, os, "Start time does not match model time.", __FILE__, __LINE__);
    }
    //
    //
    // 2.  Loop to get to ending time ----------------------------------------
    //     The loop starts here
    //
    while (ww4_utils::TimeManagement::differenceInSeconds(
               *getWaveTimeData().modelTime, endTime) > 0.001) {
      //
      // 3.  Determine time step -----------------------------------------------
      // 3.0 Step from message
      //
      if (getRunConfig().produceStdOut) {
        os << "  Step from "
           << ww4_utils::TimeManagement::toFormattedString(
                  *getWaveTimeData().modelTime)
           << std::endl;
      }
      if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
        getLogFileStream() << "  Step from "
                           << ww4_utils::TimeManagement::toFormattedString(
                                  *getWaveTimeData().modelTime)
                           << std::endl;
      }
      //
      // 3.1 Update the inputs and next time/timestep when next input is needed
      //
      if (getRunConfig().waterLevels != ww4_utils::InputFieldOption::None &&
          getRunConfig().waterLevels !=
              ww4_utils::InputFieldOption::Undefined) {
        if (getRunConfig().produceStdOut) {
          os << "    Updating water levels" << std::endl;
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          getLogFileStream() << "    Updating water levels" << std::endl;
        }
        if (getRunConfig().waterLevels ==
            ww4_utils::InputFieldOption::Homogeneous) {
          w4core_hom_water_levels(endTime);
        }
      }

      if (getRunConfig().currents != ww4_utils::InputFieldOption::None &&
          getRunConfig().currents != ww4_utils::InputFieldOption::Undefined) {
        if (getRunConfig().produceStdOut) {
          os << "    Updating currents" << std::endl;
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          getLogFileStream() << "    Updating currents" << std::endl;
        }
        if (getRunConfig().currents ==
            ww4_utils::InputFieldOption::Homogeneous) {
          w4core_hom_currents(endTime);
        }
      }

      if (getRunConfig().winds != ww4_utils::InputFieldOption::None &&
          getRunConfig().winds != ww4_utils::InputFieldOption::Undefined) {
        if (getRunConfig().produceStdOut) {
          os << "    Updating winds" << std::endl;
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          getLogFileStream() << "    Updating winds" << std::endl;
        }
        if (getRunConfig().winds == ww4_utils::InputFieldOption::Homogeneous) {
          w4core_hom_winds(endTime);
        }
      }

      if (getRunConfig().iceConcentrations !=
              ww4_utils::InputFieldOption::None &&
          getRunConfig().iceConcentrations !=
              ww4_utils::InputFieldOption::Undefined) {
        if (getRunConfig().produceStdOut) {
          os << "    Updating ice concentrations" << std::endl;
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          getLogFileStream() << "    Updating ice concentrations" << std::endl;
        }
        if (getRunConfig().iceConcentrations ==
            ww4_utils::InputFieldOption::Homogeneous) {
          w4core_hom_ice(endTime);
        }
      }
      //
      // 3.2 Find the next time/timestep for which output is requested
      //
      //
      // 3.3 Set the time step for this cycle of the time step loop
      //
      double actualTimeStep = getWaveTimeData().timeStep;

      if (getRunConfig().waterLevels != ww4_utils::InputFieldOption::None &&
          getRunConfig().waterLevels !=
              ww4_utils::InputFieldOption::Undefined) {
        if (getWaveTimeData().waterLevels.maxStep > 0.0) {
          actualTimeStep =
              std::min(actualTimeStep, getWaveTimeData().waterLevels.maxStep);
        }
      }

      if (getRunConfig().currents != ww4_utils::InputFieldOption::None &&
          getRunConfig().currents != ww4_utils::InputFieldOption::Undefined) {
        if (getWaveTimeData().currents.maxStep > 0.0) {
          actualTimeStep =
              std::min(actualTimeStep, getWaveTimeData().currents.maxStep);
        }
      }

      if (getRunConfig().winds != ww4_utils::InputFieldOption::None &&
          getRunConfig().winds != ww4_utils::InputFieldOption::Undefined) {
        if (getWaveTimeData().winds.maxStep > 0.0) {
          actualTimeStep =
              std::min(actualTimeStep, getWaveTimeData().winds.maxStep);
        }
      }

      if (getRunConfig().iceConcentrations !=
              ww4_utils::InputFieldOption::None &&
          getRunConfig().iceConcentrations !=
              ww4_utils::InputFieldOption::Undefined) {
        if (getWaveTimeData().iceConcentrations.maxStep > 0.0) {
          actualTimeStep = std::min(
              actualTimeStep, getWaveTimeData().iceConcentrations.maxStep);
        }
      }

      // Ensure actualTimeStep is at least a minimum value (e.g., 0.001) if
      // everything else is zero
      if (actualTimeStep < 0.001) {
        actualTimeStep = 0.001;
      }
      //
      // 4.  Propagate the solution (the actual model) -------------------------
      //
      // Sleep for 0.1 seconds until we have something to do here
      //
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      //
      // 5.  Placeholder for in-line data assimilation -------------------------
      //
      //
      // 6.  Perform output ----------------------------------------------------
      //
      if (getRunConfig().outputFields.requested) {
        if (getRunConfig().produceStdOut) {
          os << "    Performing fields output" << std::endl;
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          getLogFileStream() << "    Performing fields output" << std::endl;
        }
      }

      if (getRunConfig().outputPoints.requested) {
        if (getRunConfig().produceStdOut) {
          os << "    Performing points output" << std::endl;
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          getLogFileStream() << "    Performing points output" << std::endl;
        }
      }

      if (getRunConfig().outputNesting.requested) {
        if (getRunConfig().produceStdOut) {
          os << "    Performing nesting output" << std::endl;
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          getLogFileStream() << "    Performing nesting output" << std::endl;
        }
      }

      if (getRunConfig().outputTracks.requested) {
        if (getRunConfig().produceStdOut) {
          os << "    Performing tracks output" << std::endl;
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          getLogFileStream() << "    Performing tracks output" << std::endl;
        }
      }

      if (getRunConfig().outputRestart.requested) {
        if (getRunConfig().produceStdOut) {
          os << "    Performing restart output" << std::endl;
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          getLogFileStream() << "    Performing restart output" << std::endl;
        }
      }
      //
      //     End of the basic time stepping loop starting at 2 ---------------
      //
      ww4_utils::DateTime nextTime = *getWaveTimeData().modelTime;
      ww4_utils::TimeManagement::incrementDateTime(nextTime, actualTimeStep);
      updateWaveModelTime(nextTime);
    }
    //
  } catch (const std::exception &e) {
    ww4_utils::ww4_std_out::extcde(1, os, e.what(), __FILE__, __LINE__);
  } catch (...) {
    ww4_utils::ww4_std_out::extcde(1, os, "Unknown exception in w4core_wave",
                                   __FILE__, __LINE__);
  }
}

} // namespace ww4_core
