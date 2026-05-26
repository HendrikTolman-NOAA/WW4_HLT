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
 * NWS often uses Generative AI (GenAI) for code development and refactoring. Whenever GenAI is used, NWS requires a full human review of code before it is added to its repositories
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last update : 2026-05-21
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_wave.h"
#include "ww4_core/w4core_init.h"
#include "ww4_utils/time_management.h"
#include "ww4_utils/ww4_input_utils.h"
#include "ww4_utils/ww4_std_out.h"
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

/**
 * @namespace ww4_core
 * @brief Core routines for WAVEWATCH IV.
 */
namespace ww4_core {

/**
 * @brief Time stepping routine for the WAVEWATCH IV core.
 * @details Performs a single time step or a sequence of time steps for the
 *          wave model core. Follows the architectural design of the
 *          time stepping in ww3_multi.F90 from WAVEWATCH III.
 * @param[in] startTime Simulation start time.
 * @param[in] endTime Simulation end time.
 * @param[in] os Output stream for reporting.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-05-01
 */
void w4core_wave(const ww4_utils::DateTime &startTime,
                 const ww4_utils::DateTime &endTime, std::ostream &os) {
  try {
    //
    // 1.  General initialization  -------------------------------------------
    // 1.1 Output to standard output (if requested)
    //
    if (getRunConfig().produceStdOut) {
      os << "  Time stepping (w4core_wave) from: "
         << ww4_utils::TimeManagement::toFormattedString(startTime)
         << " to: " << ww4_utils::TimeManagement::toFormattedString(endTime)
         << "\n"
         << std::endl;
    }
    //
    // 1.2 Output to  log file (if requested)
    //
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
    // 1.4 Assess output configurations
    //
    ww4_utils::assessOutputConfig(startTime, endTime, getMutableRunConfig());
    //
    // 2.  Loop to get to ending time ----------------------------------------
    // 2.1 Initialize tracking of reported interpolation times
    //
    ww4_utils::InputUpdateState inputState;
    //
    // 2.2 The loop starts here
    //
    int consecutiveZeroSteps = 0;

    while (ww4_utils::TimeManagement::differenceInSeconds(
               *getWaveTimeData().modelTime, endTime) > 0.001) {
      //
      // 3.  Determine time step -----------------------------------------------
      // 3.0 Computation step message
      //
      bool headerPrinted = false;
      if (getRunConfig().produceStdOut &&
          getRunConfig().screenOutputLevel ==
              ww4_utils::ScreenOutputLevel::Full) {
        os << "  Computation step starting "
           << ww4_utils::TimeManagement::toFormattedString(
                  *getWaveTimeData().modelTime)
           << std::endl;
        headerPrinted = true;
      }
      //
      // 3.1 Update the inputs and next time/timestep when next input is needed
      //
      ww4_utils::ww4_logfile::LogTableData logData;
      ww4_utils::waveTimeData waveTime = getWaveTimeData();
      ww4_utils::updateAllInputs(*waveTime.modelTime, endTime, waveTime,
                                 inputState, getRunConfig(), headerPrinted, os,
                                 logData);

      updateWaveInputTime(ww4_utils::InputType::WaterLevels,
                          waveTime.waterLevels);
      updateWaveInputTime(ww4_utils::InputType::Currents, waveTime.currents);
      updateWaveInputTime(ww4_utils::InputType::Winds, waveTime.winds);
      updateWaveInputTime(ww4_utils::InputType::IceConcentrations,
                          waveTime.iceConcentrations);
      updateWaveInputTime(ww4_utils::InputType::BottomDepth,
                          waveTime.bottomDepth);

      double inputTimeStep =
          ww4_utils::computeInputTimeStep(*getWaveTimeData().modelTime, endTime,
                                          getWaveTimeData(), getRunConfig());

      //
      // 3.2 Find the next time/timestep for which output is requested
      //
      double outputTimeStep = ww4_utils::computeOutputTimeStep(
          *getWaveTimeData().modelTime, endTime, getRunConfig());

      //
      // 3.3 Set the time step for this cycle of the time step loop
      //
      double actualTimeStep =
          std::min({inputTimeStep, outputTimeStep, getRunConfig().timeStep});

      if (actualTimeStep < 0.0) {
        ww4_utils::ww4_std_out::extcde(1, os, "Negative time step detected.",
                                       __FILE__, __LINE__);
      } else if (actualTimeStep > 0.0 && actualTimeStep < 0.001) {
        actualTimeStep = 0.001;
      }

      if (actualTimeStep < 0.001) {
        consecutiveZeroSteps++;
      } else {
        consecutiveZeroSteps = 0;
      }

      if (consecutiveZeroSteps >= 2) {
        ww4_utils::ww4_std_out::extcde(1, os,
                                       "Two consecutive zero time steps "
                                       "detected.",
                                       __FILE__, __LINE__);
      }

      if (actualTimeStep > 0.0) {
        //
        // 4.  Propagate the solution (the actual model)
        // -------------------------
        //
        // Sleep for 0.01 seconds until we have something to do here
        //
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //
        // 4.1 Update the model time
        // --------------------------------------------
        //
        ww4_utils::DateTime nextTime = *getWaveTimeData().modelTime;
        ww4_utils::TimeManagement::incrementDateTime(nextTime, actualTimeStep);
        updateWaveModelTime(nextTime);
        //
        // 5.  Placeholder for in-line data assimilation
        // -------------------------
        //
      }
      //
      // 6.  Perform output ----------------------------------------------------
      //
      auto printOutput = [&](const std::string_view msg, bool &logFlag) {
        if (getRunConfig().produceStdOut &&
            getRunConfig().screenOutputLevel !=
                ww4_utils::ScreenOutputLevel::None) {
          if (getRunConfig().screenOutputLevel ==
                  ww4_utils::ScreenOutputLevel::Summary &&
              !headerPrinted) {
            os << "  Computation step starting "
               << ww4_utils::TimeManagement::toFormattedString(
                      *getWaveTimeData().modelTime)
               << std::endl;
            headerPrinted = true;
          }
          if (headerPrinted) {
            os << "    " << msg << std::endl;
          }
        }
        if (getRunConfig().produceLogFile && getLogFileStream().is_open()) {
          logFlag = true;
        }
      };

      if (getRunConfig().outputFields.requested &&
          getRunConfig().outputFields.actualTime ==
              getWaveTimeData().modelTime) {
        printOutput("Performing fields output", logData.fieldsPerformed);
      }

      if (getRunConfig().outputPoints.requested &&
          getRunConfig().outputPoints.actualTime ==
              getWaveTimeData().modelTime) {
        printOutput("Performing points output", logData.pointsPerformed);
      }

      if (getRunConfig().outputRestart.requested &&
          getRunConfig().outputRestart.actualTime ==
              getWaveTimeData().modelTime) {
        printOutput("Performing restart output", logData.restartPerformed);
      }

      if (getRunConfig().outputApi.requested &&
          getRunConfig().outputApi.actualTime == getWaveTimeData().modelTime) {
        printOutput("Performing API output", logData.apiPerformed);
      }

      // 6.05 Tabular log output
      if (getRunConfig().produceLogFile && getLogFileStream().is_open() &&
          logData.anyAction()) {
        ww4_utils::ww4_logfile::writeLogTableLine(
            getLogFileStream(), *getWaveTimeData().modelTime, logData);
      }

      // 6.1 Update next actual output times
      ww4_utils::updateOutputActualTimes(*getWaveTimeData().modelTime, endTime,
                                         getMutableRunConfig());
      //
      //     End of the basic time stepping loop starting at 2 ---------------
      //
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
