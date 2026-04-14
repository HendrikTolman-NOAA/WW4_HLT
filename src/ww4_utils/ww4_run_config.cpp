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
 * @date Last Update, 2026-04-13
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
std::string_view cleanValue(std::string_view s) {
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
    std::string_view lineView(line);

    // Remove comments
    const size_t hashPos = lineView.find('#');
    if (hashPos != std::string_view::npos) {
      lineView = lineView.substr(0, hashPos);
    }

    if (lineView.empty())
      continue;

    // Trim leading whitespace
    const size_t first = lineView.find_first_not_of(" \t");
    if (first == std::string_view::npos)
      continue;

    const size_t colonPos = lineView.find(':');
    if (colonPos == std::string_view::npos)
      continue;

    std::string_view key = lineView.substr(first, colonPos - first);
    // Trim trailing whitespace from key
    const size_t kend = key.find_last_not_of(" \t");
    if (kend != std::string_view::npos) {
      key = key.substr(0, kend + 1);
    }

    const std::string_view value = cleanValue(lineView.substr(colonPos + 1));

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
    } else if (key == "dry_run") {
      if (value == "yes") {
        config.dryRun = true;
      } else if (value == "no") {
        config.dryRun = false;
      }
    } else if (key == "propagate_x") {
      if (value == "yes") {
        config.propagateX = true;
      } else if (value == "no") {
        config.propagateX = false;
      }
    } else if (key == "propagate_y") {
      if (value == "yes") {
        config.propagateY = true;
      } else if (value == "no") {
        config.propagateY = false;
      }
    } else if (key == "propagate_theta") {
      if (value == "yes") {
        config.propagateTheta = true;
      } else if (value == "no") {
        config.propagateTheta = false;
      }
    } else if (key == "propagate_k") {
      if (value == "yes") {
        config.propagateK = true;
      } else if (value == "no") {
        config.propagateK = false;
      }
    } else if (key == "source_terms") {
      if (value == "yes") {
        config.sourceTerms = true;
      } else if (value == "no") {
        config.sourceTerms = false;
      }
    }
  }

  // Update TimeManagement with the loaded calendar type.
  TimeManagement::setCalendarType(config.calendarType);

  return config;
}

void reportRunConfig(const RunConfig &config, std::ostream &os) {
  os << "\n  Configuration settings :" << std::endl;

  std::string calType = "Standard";
  if (config.calendarType == TimeManagement::CalendarType::NoLeap) {
    calType = "NoLeap";
  } else if (config.calendarType ==
             TimeManagement::CalendarType::ThreeSixtyDay) {
    calType = "ThreeSixtyDay";
  }

  os << "     Calendar type      : " << calType << std::endl;
  os << "     Screen output      : " << (config.produceStdOut ? "yes" : "no")
     << std::endl;
  os << "     Log file           : " << (config.produceLogFile ? "yes" : "no")
     << std::endl;

  const bool isConventional = !config.dryRun && config.propagateX &&
                              config.propagateY && config.propagateTheta &&
                              config.propagateK && config.sourceTerms;

  os << "     Conventional model run : " << (isConventional ? "yes" : "no")
     << std::endl;

  if (!isConventional) {
    os << "        Dry run         : " << (config.dryRun ? "yes" : "no")
       << std::endl;
    os << "        Propagate X     : " << (config.propagateX ? "yes" : "no")
       << std::endl;
    os << "        Propagate Y     : " << (config.propagateY ? "yes" : "no")
       << std::endl;
    os << "        Propagate Theta : " << (config.propagateTheta ? "yes" : "no")
       << std::endl;
    os << "        Propagate K     : " << (config.propagateK ? "yes" : "no")
       << std::endl;
    os << "        Source terms    : " << (config.sourceTerms ? "yes" : "no")
       << std::endl;
  }

  os << std::endl;
}

} // namespace ww4_utils
