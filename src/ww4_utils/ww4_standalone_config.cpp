/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_standalone_config.cpp
 * @brief Implementation of stand-alone configuration processing.
 * @details This file implements the loadStandaloneConfig function,
 *          providing a simple YAML parser for start and end times.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Last update : 2026-05-20
 */

#include "ww4_utils/ww4_standalone_config.h"
#include "ww4_utils/ww4_run_config.h"
#include <charconv>
#include <fstream>
#include <string>

/**
 * @namespace ww4_utils
 * @brief Utilities for WAVEWATCH IV.
 */
namespace ww4_utils {

/**
 * @struct StandaloneConfig
 * @brief Configuration for the ww4_standalone program.
 * @details Stores the start and end times for the simulation.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @var StandaloneConfig::startTime
 * @brief Simulation start time.
 * @var StandaloneConfig::endTime
 * @brief Simulation end time.
 */

/**
 * @brief Internal helper to parse a date-time string in "YYYYMMDD HHMMSS"
 * format.
 * @param s The string view to parse.
 * @return A DateTime structure if successful, or std::nullopt.
 */
std::optional<DateTime> parseDateTimeString(const std::string_view s) {
  const std::string_view clean = cleanValue(s);

  if (clean.length() < 15 || clean[8] != ' ')
    return std::nullopt;

  int ymd = 0;
  const auto [ptr1, ec1] = std::from_chars(clean.data(), clean.data() + 8, ymd);
  if (ec1 != std::errc())
    return std::nullopt;

  const std::string_view hms_part = clean.substr(9);
  double hms = 0.0;
  if (std::from_chars(hms_part.data(), hms_part.data() + hms_part.size(), hms)
          .ec != std::errc()) {
    return std::nullopt;
  }

  return DateTime{ymd, hms};
}

/**
 * @brief Loads the stand-alone configuration from a YAML file.
 * @details Reads the specified YAML file from the current directory,
 *          extracts the start and end times, and validates that the
 *          end time is not before the start time.
 * @param filename The name of the YAML file to load.
 * @param os Output stream for reporting.
 * @return A StandaloneConfig structure if successful, or std::nullopt
 *         if an error occurred (e.g., file not found, invalid format,
 *         or validation failure).
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
std::optional<StandaloneConfig>
loadStandaloneConfig(const std::string_view filename,
                     std::ostream &os) noexcept {
  std::ifstream file((std::string(filename)));
  if (!file.is_open()) {
    os << "WW4 ERROR: Stand-alone configuration file '" << filename
       << "' not found or could not be opened." << std::endl;
    return std::nullopt;
  }

  StandaloneConfig config{};
  bool startFound = false;
  bool endFound = false;

  std::string line;
  int lineNum = 0;
  while (std::getline(file, line)) {
    lineNum++;
    const std::string_view lineFullView(line);

    // Remove comments
    const size_t hashPos = lineFullView.find('#');
    const std::string_view lineView = (hashPos != std::string_view::npos)
                                          ? lineFullView.substr(0, hashPos)
                                          : lineFullView;

    if (lineView.empty())
      continue;

    // Trim leading whitespace
    const size_t first = lineView.find_first_not_of(" \t");
    if (first == std::string_view::npos)
      continue;

    const size_t colonPos = lineView.find(':');
    if (colonPos == std::string_view::npos)
      continue;

    const std::string_view key_raw = lineView.substr(first, colonPos - first);
    // Trim trailing whitespace from key
    const size_t kend = key_raw.find_last_not_of(" \t");
    const std::string_view key = (kend != std::string_view::npos)
                                     ? key_raw.substr(0, kend + 1)
                                     : key_raw;

    const std::string_view value = lineView.substr(colonPos + 1);

    if (key == "start_time") {
      const auto dt = parseDateTimeString(value);
      if (dt) {
        config.startTime = *dt;
        startFound = true;
      } else {
        os << "WW4 ERROR: Invalid start_time format in '" << filename
           << "' at line " << lineNum << ": " << value << std::endl;
        os << "           Expected format: \"YYYYMMDD HHMMSS\"" << std::endl;
      }
    } else if (key == "end_time") {
      const auto dt = parseDateTimeString(value);
      if (dt) {
        config.endTime = *dt;
        endFound = true;
      } else {
        os << "WW4 ERROR: Invalid end_time format in '" << filename
           << "' at line " << lineNum << ": " << value << std::endl;
        os << "           Expected format: \"YYYYMMDD HHMMSS\"" << std::endl;
      }
    }
  }

  if (!startFound || !endFound) {
    os << "WW4 ERROR: Mandatory field(s) missing in '" << filename
       << "':" << std::endl;
    if (!startFound)
      os << "           Missing: start_time" << std::endl;
    if (!endFound)
      os << "           Missing: end_time" << std::endl;
    return std::nullopt;
  }

  // Validation: endTime >= startTime
  if (TimeManagement::differenceInSeconds(config.startTime, config.endTime) <
      0.0) {
    os << "WW4 ERROR: End time before start time in '" << filename
       << "':" << std::endl;
    os << "           Start time: "
       << TimeManagement::toFormattedString(config.startTime) << std::endl;
    os << "           End time:   "
       << TimeManagement::toFormattedString(config.endTime) << std::endl;
    return std::nullopt;
  }

  return config;
}

/**
 * @brief Reports the stand-alone configuration to the provided output stream.
 * @param config The StandaloneConfig structure to report.
 * @param os The output stream to write to (default: std::cout).
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
void reportStandaloneConfig(const StandaloneConfig &config, std::ostream &os) {
  os << "\n  Stand-alone configuration settings :" << std::endl;
  os << "     Start time         : "
     << TimeManagement::toFormattedString(config.startTime) << std::endl;
  os << "     End time           : "
     << TimeManagement::toFormattedString(config.endTime) << std::endl;
  os << std::endl;
}

} // namespace ww4_utils
