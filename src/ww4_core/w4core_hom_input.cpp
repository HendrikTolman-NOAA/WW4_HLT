/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_hom_input.cpp
 * @brief Implementation of homogeneous input data processing for WW4 core.
 * @details This file implements the w4core_hom_input routine and its helpers.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#include "ww4_core/w4core_hom_input.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_core/w4core_wave.hpp"
#include "ww4_utils/ww4_stand_alone_config.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <charconv>
#include <iostream>
#include <optional>
#include <sstream>

namespace ww4_core {

using namespace ww4_utils;

namespace {

std::vector<HomogeneousDataPoint> waterLevels;
std::vector<HomogeneousDataPoint> currents;
std::vector<HomogeneousDataPoint> winds;
std::vector<HomogeneousDataPoint> iceConcentrations;
std::vector<HomogeneousDataPoint> bottomDepth;

/**
 * @brief Helper to validate and process a data series.
 * @param source Vector of pre-parsed data points from RunConfig.
 * @param processed Vector to store validated data points.
 * @param fieldName Name of the field for error reporting.
 * @param option The input option for this field.
 * @param os Output stream for reporting.
 */
void processSeries(const std::vector<HomogeneousDataPoint> &source,
                   std::vector<HomogeneousDataPoint> &processed,
                   std::string_view fieldName,
                   ww4_utils::InputFieldOption option, std::ostream &os) {

  if (option != ww4_utils::InputFieldOption::Homogeneous) {
    return;
  }

  if (source.empty()) {
    ww4_utils::ww4_std_out::extcde(1, os,
                                   "No data provided for homogeneous field: " +
                                       std::string(fieldName),
                                   __FILE__, __LINE__);
  }

  for (const auto &dp : source) {
    if (!processed.empty()) {
      const double diff = ww4_utils::TimeManagement::differenceInSeconds(
          processed.back().time, dp.time);
      if (diff < 0.0) {
        ww4_utils::ww4_std_out::extcde(1, os,
                                       "Time stamps go backward in data for " +
                                           std::string(fieldName),
                                       __FILE__, __LINE__);
      }
    }

    // Field-specific validation
    if (fieldName == "water levels" || fieldName == "bottom depth") {
      if (dp.values.size() != 1) {
        ww4_utils::ww4_std_out::extcde(1, os,
                                       "Homogeneous " + std::string(fieldName) +
                                           " requires 1 value.",
                                       __FILE__, __LINE__);
      }
    } else if (fieldName == "currents") {
      if (dp.values.size() != 2) {
        ww4_utils::ww4_std_out::extcde(
            1, os, "Homogeneous currents requires 2 values (speed, direction).",
            __FILE__, __LINE__);
      }
    } else if (fieldName == "winds") {
      if (dp.values.size() < 2 || dp.values.size() > 3) {
        ww4_utils::ww4_std_out::extcde(1, os,
                                       "Homogeneous winds requires 2 or 3 "
                                       "values (speed, direction, [temp]).",
                                       __FILE__, __LINE__);
      }
    } else if (fieldName == "ice concentrations") {
      if (dp.values.size() != 1) {
        ww4_utils::ww4_std_out::extcde(1, os,
                                       "Homogeneous ice concentrations "
                                       "requires 1 value.",
                                       __FILE__, __LINE__);
      }
      if (dp.values[0] < 0.0 || dp.values[0] > 1.0) {
        ww4_utils::ww4_std_out::extcde(
            1, os, "Ice concentration must be between 0.0 and 1.0.", __FILE__,
            __LINE__);
      }
    }

    processed.push_back(dp);
  }
}

/**
 * @brief Helper to cycle through homogeneous input data.
 * @param series The vector of homogeneous data points.
 * @param type The input type to update.
 * @param endTime Simulation end time for capping max step.
 */
void updateHomogeneousInputCycling(
    const std::vector<HomogeneousDataPoint> &series, InputType type,
    const DateTime &endTime) {

  const auto &waveTimeData = getWaveTimeData();
  if (!waveTimeData.modelTime.has_value()) {
    return;
  }
  const DateTime modelTime = *waveTimeData.modelTime;

  InputTimeData data;

  if (series.empty()) {
    data.time1 = modelTime;
    data.time2 = endTime;
    data.maxStep = TimeManagement::differenceInSeconds(modelTime, endTime);
    updateWaveInputTime(type, data);
    return;
  }

  // Find the interval around the present model time.
  // The first and second time tags should be around the present model time,
  // where the first time tag can be equal to the model time.

  // Case 1: If the first time of the homogeneous input is after the present
  // model time, set the first time to the present model time, and the second
  // time to the first time for which the homogeneous input is defined.
  if (TimeManagement::differenceInSeconds(modelTime, series.front().time) >
      0.001) {
    data.time1 = modelTime;
    data.time2 = series.front().time;
  }
  // Case 2: If the last time for the input is before the model time,
  // set the second time to the ending time of the run.
  // (time1 will be the last data point time).
  else if (TimeManagement::differenceInSeconds(series.back().time, modelTime) >
           -0.001) {
    data.time1 = series.back().time;
    data.time2 = endTime;
  }
  // Case 3: Model time is within the range of the input data.
  else {
    for (size_t i = 0; i < series.size() - 1; ++i) {
      if (TimeManagement::differenceInSeconds(series[i].time, modelTime) >=
              -0.001 &&
          TimeManagement::differenceInSeconds(modelTime, series[i + 1].time) >
              0.001) {
        data.time1 = series[i].time;
        data.time2 = series[i + 1].time;
        break;
      }
    }
  }

  // Ensure time2 is not before modelTime
  if (data.time2.has_value() &&
      TimeManagement::differenceInSeconds(modelTime, *data.time2) < 0.0) {
    data.time2 = endTime;
  }

  // Calculate maxStep: the time interval from the present model time to the
  // second time tag.
  if (data.time2.has_value()) {
    data.maxStep = TimeManagement::differenceInSeconds(modelTime, *data.time2);
  } else {
    data.maxStep = TimeManagement::differenceInSeconds(modelTime, endTime);
    data.time2 = endTime;
  }

  updateWaveInputTime(type, data);
}

} // namespace

void w4core_hom_input(std::ostream &os) {
  const auto &config = getRunConfig();

  // Reset before processing
  resetInputData();

  processSeries(config.homogeneousWaterLevels, waterLevels, "water levels",
                config.waterLevels, os);
  processSeries(config.homogeneousCurrents, currents, "currents",
                config.currents, os);
  processSeries(config.homogeneousWinds, winds, "winds", config.winds, os);
  processSeries(config.homogeneousIceConcentrations, iceConcentrations,
                "ice concentrations", config.iceConcentrations, os);
  processSeries(config.homogeneousBottomDepth, bottomDepth, "bottom depth",
                config.bottomDepth, os);
}

void resetInputData() noexcept {
  waterLevels.clear();
  currents.clear();
  winds.clear();
  iceConcentrations.clear();
  bottomDepth.clear();
}

const std::vector<HomogeneousDataPoint> &getHomogeneousWaterLevels() noexcept {
  return waterLevels;
}

const std::vector<HomogeneousDataPoint> &getHomogeneousCurrents() noexcept {
  return currents;
}

const std::vector<HomogeneousDataPoint> &getHomogeneousWinds() noexcept {
  return winds;
}

const std::vector<HomogeneousDataPoint> &
getHomogeneousIceConcentrations() noexcept {
  return iceConcentrations;
}

const std::vector<HomogeneousDataPoint> &getHomogeneousBottomDepth() noexcept {
  return bottomDepth;
}

void w4core_hom_water_levels(const DateTime &endTime) {
  updateHomogeneousInputCycling(waterLevels, InputType::WaterLevels, endTime);
}

void w4core_hom_currents(const DateTime &endTime) {
  updateHomogeneousInputCycling(currents, InputType::Currents, endTime);
}

void w4core_hom_winds(const DateTime &endTime) {
  updateHomogeneousInputCycling(winds, InputType::Winds, endTime);
}

void w4core_hom_ice(const DateTime &endTime) {
  updateHomogeneousInputCycling(iceConcentrations, InputType::IceConcentrations,
                                endTime);
}

} // namespace ww4_core
