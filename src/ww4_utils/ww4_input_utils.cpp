/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_input_utils.cpp
 * @brief Implementation of model input orchestration routines.
 * @details This file implements the updateAllInputs routine for WW4.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#include "ww4_utils/ww4_input_utils.hpp"
#include "ww4_utils/ww4_input_update.hpp"
#include "ww4_utils/ww4_logfile.hpp"
#include "ww4_utils/ww4_run_config.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <algorithm>

namespace ww4_utils {

namespace {

/**
 * @brief Helper to process a single input field.
 * @param[in] fieldName Name of the field for logging.
 * @param[in] option Input option for this field.
 * @param[in] modelTime Current model time.
 * @param[in] endTime Simulation end time.
 * @param[in,out] data Data structure for this field.
 * @param[in,out] lastTime1 Tracker for last reported time1.
 * @param[in,out] lastTime2 Tracker for last reported time2.
 * @param[in] produceStdOut Flag to produce standard output.
 * @param[in,out] os Standard output stream.
 * @param[in] produceLogFile Flag to produce log file output.
 * @param[in,out] logStream Log file stream.
 */
void processField(std::string_view fieldName, InputFieldOption option,
                  const DateTime &modelTime, const DateTime &endTime,
                  intTimeData &data, std::optional<DateTime> &lastTime1,
                  std::optional<DateTime> &lastTime2, bool produceStdOut,
                  std::ostream &os, bool produceLogFile,
                  std::ostream &logStream) {

  if (option == InputFieldOption::None ||
      option == InputFieldOption::Undefined) {
    return;
  }

  const bool skipUpdateMessage =
      (fieldName == "bottom depth" && option == InputFieldOption::FromGrid);

  if (!skipUpdateMessage) {
    if (produceStdOut) {
      ww4_std_out::writeUpdatingField(os, fieldName);
    }
    if (produceLogFile) {
      ww4_logfile::writeUpdatingField(logStream, fieldName);
    }
  }

  if (option == InputFieldOption::Homogeneous) {
    if (fieldName == "water levels") {
      ww4_hom_water_levels(modelTime, endTime, data);
    } else if (fieldName == "currents") {
      ww4_hom_currents(modelTime, endTime, data);
    } else if (fieldName == "winds") {
      ww4_hom_winds(modelTime, endTime, data);
    } else if (fieldName == "ice concentrations") {
      ww4_hom_ice(modelTime, endTime, data);
    } else if (fieldName == "bottom depth") {
      ww4_hom_bottom_depth(modelTime, endTime, data);
    }

    if (data.time1.has_value() && data.time2.has_value()) {
      if (data.time1 != lastTime1 || data.time2 != lastTime2) {
        if (produceStdOut) {
          ww4_std_out::writeInterpolationInfo(os, *data.time1, *data.time2);
        }
        if (produceLogFile) {
          ww4_logfile::writeInterpolationInfo(logStream, *data.time1,
                                              *data.time2);
        }
        lastTime1 = data.time1;
        lastTime2 = data.time2;
      }
    }
  }
}

} // namespace

double updateAllInputs(const DateTime &modelTime, const DateTime &endTime,
                       waveTimeData &waveTime, InputUpdateState &state,
                       const RunConfig &config, std::ostream &os,
                       std::ostream &logStream) {

  processField("water levels", config.waterLevels, modelTime, endTime,
               waveTime.waterLevels, state.lastWlTime1, state.lastWlTime2,
               config.produceStdOut, os, config.produceLogFile, logStream);

  processField("currents", config.currents, modelTime, endTime,
               waveTime.currents, state.lastCuTime1, state.lastCuTime2,
               config.produceStdOut, os, config.produceLogFile, logStream);

  processField("winds", config.winds, modelTime, endTime, waveTime.winds,
               state.lastWiTime1, state.lastWiTime2, config.produceStdOut, os,
               config.produceLogFile, logStream);

  processField("ice concentrations", config.iceConcentrations, modelTime,
               endTime, waveTime.iceConcentrations, state.lastIcTime1,
               state.lastIcTime2, config.produceStdOut, os,
               config.produceLogFile, logStream);

  processField("bottom depth", config.bottomDepth, modelTime, endTime,
               waveTime.bottomDepth, state.lastBdTime1, state.lastBdTime2,
               config.produceStdOut, os, config.produceLogFile, logStream);

  // Calculate minimum maxStep for inputs only
  double inputTimeStep =
      TimeManagement::differenceInSeconds(modelTime, endTime);

  if (config.waterLevels != InputFieldOption::None &&
      config.waterLevels != InputFieldOption::Undefined) {
    if (waveTime.waterLevels.maxStep > 0.0) {
      inputTimeStep = std::min(inputTimeStep, waveTime.waterLevels.maxStep);
    }
  }

  if (config.currents != InputFieldOption::None &&
      config.currents != InputFieldOption::Undefined) {
    if (waveTime.currents.maxStep > 0.0) {
      inputTimeStep = std::min(inputTimeStep, waveTime.currents.maxStep);
    }
  }

  if (config.winds != InputFieldOption::None &&
      config.winds != InputFieldOption::Undefined) {
    if (waveTime.winds.maxStep > 0.0) {
      inputTimeStep = std::min(inputTimeStep, waveTime.winds.maxStep);
    }
  }

  if (config.iceConcentrations != InputFieldOption::None &&
      config.iceConcentrations != InputFieldOption::Undefined) {
    if (waveTime.iceConcentrations.maxStep > 0.0) {
      inputTimeStep =
          std::min(inputTimeStep, waveTime.iceConcentrations.maxStep);
    }
  }

  if (config.bottomDepth != InputFieldOption::None &&
      config.bottomDepth != InputFieldOption::Undefined) {
    if (waveTime.bottomDepth.maxStep > 0.0) {
      inputTimeStep = std::min(inputTimeStep, waveTime.bottomDepth.maxStep);
    }
  }

  if (inputTimeStep < 0.0) {
    inputTimeStep = 0.0;
  }

  return inputTimeStep;
}

} // namespace ww4_utils
