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
 * @date 2026-04-17
 */

#include "ww4_core/w4core_hom_input.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_utils/ww4_stand_alone_config.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <charconv>
#include <iostream>
#include <optional>
#include <sstream>

namespace ww4_core {

namespace {

std::vector<ww4_utils::HomogeneousDataPoint> waterLevels;
std::vector<ww4_utils::HomogeneousDataPoint> currents;
std::vector<ww4_utils::HomogeneousDataPoint> winds;
std::vector<ww4_utils::HomogeneousDataPoint> iceConcentrations;
std::vector<ww4_utils::HomogeneousDataPoint> bottomDepth;

/**
 * @brief Helper to validate a data series.
 * @param processed Vector of data points.
 * @param fieldName Name of the field for error reporting.
 * @param option The input option for this field.
 * @param minValues Minimum number of values per data point.
 * @param maxValues Maximum number of values per data point.
 * @param os Output stream for reporting.
 */
void validateSeries(const std::vector<ww4_utils::HomogeneousDataPoint> &data,
                    std::vector<ww4_utils::HomogeneousDataPoint> &target,
                    std::string_view fieldName,
                    ww4_utils::InputFieldOption option, size_t minValues,
                    size_t maxValues, std::ostream &os) {

  if (option != ww4_utils::InputFieldOption::Homogeneous) {
    return;
  }

  if (data.empty()) {
    ww4_utils::ww4_std_out::extcde(1, os,
                                   "No data provided for homogeneous field: " +
                                       std::string(fieldName),
                                   __FILE__, __LINE__);
  }

  for (size_t i = 0; i < data.size(); ++i) {
    const auto &dp = data[i];

    if (dp.values.size() < minValues || dp.values.size() > maxValues) {
      ww4_utils::ww4_std_out::extcde(
          1, os,
          "Invalid number of values for homogeneous field " +
              std::string(fieldName) + " at time " +
              ww4_utils::TimeManagement::toFormattedString(dp.time),
          __FILE__, __LINE__);
    }

    if (i > 0) {
      const double diff = ww4_utils::TimeManagement::differenceInSeconds(
          data[i - 1].time, dp.time);
      if (diff < 0.0) {
        ww4_utils::ww4_std_out::extcde(1, os,
                                       "Time stamps go backward in data for " +
                                           std::string(fieldName),
                                       __FILE__, __LINE__);
      }
    }

    // Special validation for ice concentration
    if (fieldName == "ice concentrations") {
      if (dp.values[0] < 0.0 || dp.values[0] > 1.0) {
        ww4_utils::ww4_std_out::extcde(
            1, os,
            "Ice concentration must be between 0.0 and 1.0. Found: " +
                std::to_string(dp.values[0]),
            __FILE__, __LINE__);
      }
    }

    target.push_back(dp);
  }
}

} // namespace

void w4core_hom_input(std::ostream &os) {
  const auto &config = getRunConfig();

  // Reset before processing
  resetInputData();

  validateSeries(config.homogeneousWaterLevels, waterLevels, "water levels",
                 config.waterLevels, 1, 1, os);
  validateSeries(config.homogeneousCurrents, currents, "currents",
                 config.currents, 2, 2, os);
  validateSeries(config.homogeneousWinds, winds, "winds", config.winds, 2, 3,
                 os);
  validateSeries(config.homogeneousIceConcentrations, iceConcentrations,
                 "ice concentrations", config.iceConcentrations, 1, 1, os);
  validateSeries(config.homogeneousBottomDepth, bottomDepth, "bottom depth",
                 config.bottomDepth, 1, 1, os);
}

void echoInputData(std::ostream &, ww4_utils::EchoOption) {
  // Echoing is now handled by reportRunConfig in ww4_utils.
}

void resetInputData() noexcept {
  waterLevels.clear();
  currents.clear();
  winds.clear();
  iceConcentrations.clear();
  bottomDepth.clear();
}

const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousWaterLevels() noexcept {
  return waterLevels;
}

const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousCurrents() noexcept {
  return currents;
}

const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousWinds() noexcept {
  return winds;
}

const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousIceConcentrations() noexcept {
  return iceConcentrations;
}

const std::vector<ww4_utils::HomogeneousDataPoint> &
getHomogeneousBottomDepth() noexcept {
  return bottomDepth;
}

} // namespace ww4_core
