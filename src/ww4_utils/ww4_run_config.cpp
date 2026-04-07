/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_run_config.cpp
 * @brief Implementation of run-time configuration processing.
 * @details This file implements the loadRunConfig function,
 *          providing a simple YAML parser for run-time settings.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last Update, 2026-04-03
 */

#include "ww4_utils/ww4_run_config.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace ww4_utils {

/**
 * @brief Internal helper to trim whitespace and quotes from a string.
 * @param s The string view to clean.
 * @return A cleaned string view.
 */
static std::string_view cleanValue(std::string_view s) {
  const size_t start = s.find_first_not_of(" \t\"");
  const size_t end = s.find_last_not_of(" \t\"");
  if (start == std::string_view::npos)
    return "";
  return s.substr(start, end - start + 1);
}

std::optional<RunConfig> loadRunConfig(std::string_view filename) noexcept {
  std::ifstream file((std::string(filename)));
  RunConfig config{};

  if (!file.is_open()) {
    return std::nullopt;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty())
      continue;

    // Trim leading whitespace
    const size_t first = line.find_first_not_of(" \t");
    if (first == std::string::npos || line[first] == '#')
      continue;

    const size_t colonPos = line.find(':');
    if (colonPos == std::string::npos)
      continue;

    std::string_view key =
        std::string_view(line).substr(first, colonPos - first);
    // Trim trailing whitespace from key
    const size_t kend = key.find_last_not_of(" \t");
    if (kend != std::string_view::npos) {
      key = key.substr(0, kend + 1);
    }

    const std::string_view value =
        cleanValue(std::string_view(line).substr(colonPos + 1));

    if (key == "calendar_type") {
      if (value == "Standard") {
        config.calendarType = TimeManagement::CalendarType::Standard;
      } else if (value == "NoLeap") {
        config.calendarType = TimeManagement::CalendarType::NoLeap;
      } else if (value == "ThreeSixtyDay") {
        config.calendarType = TimeManagement::CalendarType::ThreeSixtyDay;
      }
    } else if (key == "produce_std_out") {
      if (value == "yes") {
        config.produceStdOut = true;
      } else if (value == "no") {
        config.produceStdOut = false;
      }
    } else if (key == "produce_log_file") {
      if (value == "yes") {
        config.produceLogFile = true;
      } else if (value == "no") {
        config.produceLogFile = false;
      }
    }
  }

  // Update TimeManagement with the loaded calendar type.
  TimeManagement::setCalendarType(config.calendarType);

  return config;
}

void reportRunConfig(const RunConfig &config, std::ostream &os) {
  os << "          Configuration settings :" << std::endl;

  std::string calType = "Standard";
  if (config.calendarType == TimeManagement::CalendarType::NoLeap) {
    calType = "NoLeap";
  } else if (config.calendarType ==
             TimeManagement::CalendarType::ThreeSixtyDay) {
    calType = "ThreeSixtyDay";
  }

  os << "            Calendar type      : " << calType << std::endl;
  os << "            Screen output      : "
     << (config.produceStdOut ? "yes" : "no") << std::endl;
  os << "            Log file           : "
     << (config.produceLogFile ? "yes" : "no") << std::endl;
  os << std::endl;
}

} // namespace ww4_utils
