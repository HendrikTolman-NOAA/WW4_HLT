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

std::vector<HomogeneousDataPoint> waterLevels;
std::vector<HomogeneousDataPoint> currents;
std::vector<HomogeneousDataPoint> winds;
std::vector<HomogeneousDataPoint> iceConcentrations;
std::vector<HomogeneousDataPoint> bottomDepth;

/**
 * @brief Helper to parse a homogeneous data string.
 * @param s The string to parse (format: "YYYYMMDD HHMMSS val1 val2 ...").
 * @return A HomogeneousDataPoint if successful.
 */
std::optional<HomogeneousDataPoint> parseHomogeneousString(std::string_view s) {
  if (s.empty())
    return std::nullopt;

  const size_t firstSpace = s.find(' ');
  if (firstSpace == std::string_view::npos)
    return std::nullopt;

  const size_t secondSpace = s.find(' ', firstSpace + 1);
  if (secondSpace == std::string_view::npos)
    return std::nullopt;

  const std::string_view dateTimePart = s.substr(0, secondSpace);
  const auto dt = ww4_utils::parseDateTimeString(dateTimePart);
  if (!dt)
    return std::nullopt;

  HomogeneousDataPoint dp;
  dp.time = *dt;

  std::string_view remaining = s.substr(secondSpace);
  while (!remaining.empty()) {
    const size_t firstNotSpace = remaining.find_first_not_of(' ');
    if (firstNotSpace == std::string_view::npos)
      break;
    remaining = remaining.substr(firstNotSpace);
    const size_t nextSpace = remaining.find(' ');
    const std::string_view valStr = remaining.substr(0, nextSpace);

    double val = 0.0;
    if (std::from_chars(valStr.data(), valStr.data() + valStr.size(), val).ec ==
        std::errc()) {
      dp.values.push_back(val);
    } else {
      return std::nullopt;
    }

    if (nextSpace == std::string_view::npos)
      break;
    remaining = remaining.substr(nextSpace);
  }

  if (dp.values.empty())
    return std::nullopt;

  return dp;
}

/**
 * @brief Helper to process a data series.
 * @param rawStrings Vector of raw input strings.
 * @param processed Vector to store parsed data points.
 * @param fieldName Name of the field for error reporting.
 * @param option The input option for this field.
 * @param os Output stream for reporting.
 */
void processSeries(const std::vector<std::string> &rawStrings,
                   std::vector<HomogeneousDataPoint> &processed,
                   std::string_view fieldName,
                   ww4_utils::InputFieldOption option, std::ostream &os) {

  if (option != ww4_utils::InputFieldOption::Homogeneous) {
    return;
  }

  for (const auto &raw : rawStrings) {
    auto dp = parseHomogeneousString(raw);
    if (!dp) {
      ww4_utils::ww4_std_out::extcde(1, os,
                                     "Failed to parse homogeneous data for " +
                                         std::string(fieldName),
                                     __FILE__, __LINE__);
    }

    if (!processed.empty()) {
      const double diff = ww4_utils::TimeManagement::differenceInSeconds(
          processed.back().time, dp->time);
      if (diff < 0.0) {
        ww4_utils::ww4_std_out::extcde(1, os,
                                       "Time stamps go backward in data for " +
                                           std::string(fieldName),
                                       __FILE__, __LINE__);
      }
    }
    processed.push_back(*dp);
  }

  if (processed.empty()) {
    ww4_utils::ww4_std_out::extcde(1, os,
                                   "No data provided for homogeneous field: " +
                                       std::string(fieldName),
                                   __FILE__, __LINE__);
  }
}

/**
 * @brief Helper to echo a data series.
 * @param processed Vector of data points.
 * @param fieldName Name of the field.
 * @param option Echo level.
 * @param os Output stream.
 */
void echoSeries(const std::vector<HomogeneousDataPoint> &processed,
                std::string_view fieldName, ww4_utils::EchoOption option,
                std::ostream &os) {
  if (processed.empty() || option == ww4_utils::EchoOption::None)
    return;

  os << "     Homogeneous data for " << fieldName << ":" << std::endl;
  if (option == ww4_utils::EchoOption::Summary) {
    os << "        Number of data points: " << processed.size() << std::endl;
  } else if (option == ww4_utils::EchoOption::Full) {
    for (const auto &dp : processed) {
      os << "        " << ww4_utils::TimeManagement::toFormattedString(dp.time)
         << " :";
      for (const auto val : dp.values) {
        os << " " << val;
      }
      os << std::endl;
    }
  }
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

  if (config.produceStdOut) {
    echoInputData(os, config.echoHomInput);
  }
}

void echoInputData(std::ostream &os, ww4_utils::EchoOption option) {
  if (option == ww4_utils::EchoOption::None)
    return;

  os << "\n  Input data (w4core_hom_input) processing:" << std::endl;
  echoSeries(waterLevels, "water levels", option, os);
  echoSeries(currents, "currents", option, os);
  echoSeries(winds, "winds", option, os);
  echoSeries(iceConcentrations, "ice concentrations", option, os);
  echoSeries(bottomDepth, "bottom depth", option, os);
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

} // namespace ww4_core
