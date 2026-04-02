/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file stand_alone_config.cpp
 * @brief Implementation of stand-alone configuration processing.
 * @details This file implements the loadStandAloneConfig function,
 *          providing a simple YAML parser for start and end times.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-02
 * @date Last Update, 2026-04-02
 */

#include "ww4_utils/stand_alone_config.hpp"
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
static std::optional<DateTime> parseDateTimeString(std::string_view s) {
  const size_t start = s.find_first_not_of(" \t\"");
  const size_t end = s.find_last_not_of(" \t\"");
  if (start == std::string_view::npos)
    return std::nullopt;
  const std::string_view clean = s.substr(start, end - start + 1);

  if (clean.length() < 15 || clean[8] != ' ')
    return std::nullopt;

  int ymd = 0;
  const auto [ptr1, ec1] = std::from_chars(clean.data(), clean.data() + 8, ymd);
  if (ec1 != std::errc())
    return std::nullopt;

  const std::string_view hms_part = clean.substr(9);
  double hms = 0.0;
  try {
    hms = std::stod(std::string(hms_part));
  } catch (...) {
    return std::nullopt;
  }

  return DateTime{ymd, hms};
}

std::optional<StandAloneConfig>
loadStandAloneConfig(std::string_view filename) noexcept {
  std::ifstream file((std::string(filename)));
  if (!file.is_open())
    return std::nullopt;

  StandAloneConfig config{};
  bool startFound = false;
  bool endFound = false;

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

    const std::string_view value = std::string_view(line).substr(colonPos + 1);

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

} // namespace ww4_utils
