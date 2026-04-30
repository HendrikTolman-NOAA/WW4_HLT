/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_stand_alone_config.cpp
 * @brief Implementation of stand-alone configuration processing.
 * @details This file implements the loadStandAloneConfig function,
 *          providing a simple YAML parser for start and end times.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-02
 * @date Last update, 2026-04-16
 */

#include "ww4_utils/ww4_stand_alone_config.h"
#include "ww4_utils/ww4_run_config.h"
#include <charconv>
#include <fstream>
#include <string>

namespace ww4_utils {

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

std::optional<StandAloneConfig>
loadStandAloneConfig(const std::string_view filename,
                     [[maybe_unused]] std::ostream &os) noexcept {
  std::ifstream file((std::string(filename)));
  if (!file.is_open())
    return std::nullopt;

  StandAloneConfig config{};
  bool startFound = false;
  bool endFound = false;

  std::string line;
  while (std::getline(file, line)) {
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
    if (first == std::string::npos)
      continue;

    const size_t colonPos = lineView.find(':');
    if (colonPos == std::string::npos)
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
      }
    } else if (key == "end_time") {
      const auto dt = parseDateTimeString(value);
      if (dt) {
        config.endTime = *dt;
        endFound = true;
      }
    }
  }

  if (!startFound || !endFound)
    return std::nullopt;

  // Validation: endTime >= startTime
  if (TimeManagement::differenceInSeconds(config.startTime, config.endTime) <
      0.0) {
    return std::nullopt;
  }

  return config;
}

void reportStandAloneConfig(const StandAloneConfig &config, std::ostream &os) {
  os << "\n  Stand-alone configuration settings :" << std::endl;
  os << "     Start time         : "
     << TimeManagement::toFormattedString(config.startTime) << std::endl;
  os << "     End time           : "
     << TimeManagement::toFormattedString(config.endTime) << std::endl;
  os << std::endl;
}

} // namespace ww4_utils
