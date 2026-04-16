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
 * @date Last update, 2026-04-16
 */

#include "ww4_utils/ww4_run_config.hpp"
#include "ww4_utils/ww4_stand_alone_config.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace ww4_utils {

namespace {

/**
 * @brief Helper to parse InputFieldOption from string.
 * @param value The string value to parse.
 * @param allowFromGrid Whether to allow the 'from_grid' option.
 * @return The corresponding InputFieldOption.
 */
InputFieldOption parseInputOption(std::string_view value,
                                  bool allowFromGrid = false) {
  if (value == "none") {
    return InputFieldOption::None;
  } else if (value == "from_file") {
    return InputFieldOption::FromFile;
  } else if (value == "from_coupling") {
    return InputFieldOption::FromCoupling;
  } else if (value == "homogeneous") {
    return InputFieldOption::Homogeneous;
  } else if (value == "from_grid") {
    if (allowFromGrid) {
      return InputFieldOption::FromGrid;
    }
  }
  return InputFieldOption::Undefined;
}

/**
 * @brief Helper to convert InputFieldOption to string for reporting.
 * @param option The InputFieldOption to convert.
 * @return A string representation of the option.
 */
std::string inputOptionToString(InputFieldOption option) {
  switch (option) {
  case InputFieldOption::None:
    return "none";
  case InputFieldOption::FromFile:
    return "from_file";
  case InputFieldOption::FromCoupling:
    return "from_coupling";
  case InputFieldOption::Homogeneous:
    return "homogeneous";
  case InputFieldOption::FromGrid:
    return "from_grid";
  default:
    return "undefined";
  }
}

/**
 * @brief Helper to update OutputConfig from a key-value pair.
 * @param oc The OutputConfig structure to update.
 * @param key_suffix The suffix of the configuration key.
 * @param value The value to set.
 */
void updateOutputConfig(OutputConfig &oc, std::string_view key_suffix,
                        std::string_view value) {
  if (key_suffix == "requested") {
    oc.requested = (value == "yes");
  } else if (key_suffix == "start") {
    oc.startTime = parseDateTimeString(value);
  } else if (key_suffix == "end") {
    oc.endTime = parseDateTimeString(value);
  } else if (key_suffix == "interval") {
    try {
      oc.interval = std::stod(std::string(value));
    } catch (...) {
      oc.interval = -1.0;
    }
  } else if (key_suffix == "at_first") {
    if (value == "yes") {
      oc.atFirstTime = true;
    } else if (value == "no") {
      oc.atFirstTime = false;
    }
  }
}

/**
 * @brief Helper to report OutputConfig settings.
 * @param oc The OutputConfig structure to report.
 * @param label The label for the output type.
 * @param os The output stream to write to.
 */
void reportOutput(const OutputConfig &oc, std::string_view label,
                  std::ostream &os) {
  os << "     " << label << " output : " << (oc.requested ? "yes" : "no")
     << std::endl;
  if (oc.requested) {
    os << "        Interval        : " << oc.interval << " s" << std::endl;
    os << "        At first time   : " << (oc.atFirstTime ? "yes" : "no")
       << std::endl;
    if (oc.startTime) {
      os << "        Start time      : "
         << TimeManagement::toFormattedString(*oc.startTime) << std::endl;
    }
    if (oc.endTime) {
      os << "        End time        : "
         << TimeManagement::toFormattedString(*oc.endTime) << std::endl;
    }
  }
}

} // namespace

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
    } else if (key == "water_levels") {
      config.waterLevels = parseInputOption(value);
    } else if (key == "currents") {
      config.currents = parseInputOption(value);
    } else if (key == "winds") {
      config.winds = parseInputOption(value);
    } else if (key == "ice_concentrations") {
      config.iceConcentrations = parseInputOption(value);
    } else if (key == "bottom_depth") {
      config.bottomDepth = parseInputOption(value, true);
    } else if (key == "time_step") {
      try {
        config.timeStep = std::stod(std::string(value));
      } catch (...) {
        config.timeStep = -1.0;
      }
    } else if (key.starts_with("output_fields_")) {
      updateOutputConfig(config.outputFields, key.substr(14), value);
    } else if (key.starts_with("output_points_")) {
      updateOutputConfig(config.outputPoints, key.substr(14), value);
    } else if (key.starts_with("output_nesting_")) {
      updateOutputConfig(config.outputNesting, key.substr(15), value);
    } else if (key.starts_with("output_tracks_")) {
      updateOutputConfig(config.outputTracks, key.substr(14), value);
    } else if (key.starts_with("output_restart_")) {
      updateOutputConfig(config.outputRestart, key.substr(15), value);
    }
  }

  // Mandatory fields check
  if (config.waterLevels == InputFieldOption::Undefined ||
      config.currents == InputFieldOption::Undefined ||
      config.winds == InputFieldOption::Undefined ||
      config.iceConcentrations == InputFieldOption::Undefined ||
      config.bottomDepth == InputFieldOption::Undefined) {
    std::cerr << "WW4 ERROR: Mandatory model input field(s) missing or invalid "
                 "in configuration."
              << std::endl;
    if (config.waterLevels == InputFieldOption::Undefined)
      std::cerr << "   Missing/invalid: water_levels" << std::endl;
    if (config.currents == InputFieldOption::Undefined)
      std::cerr << "   Missing/invalid: currents" << std::endl;
    if (config.winds == InputFieldOption::Undefined)
      std::cerr << "   Missing/invalid: winds" << std::endl;
    if (config.iceConcentrations == InputFieldOption::Undefined)
      std::cerr << "   Missing/invalid: ice_concentrations" << std::endl;
    if (config.bottomDepth == InputFieldOption::Undefined)
      std::cerr << "   Missing/invalid: bottom_depth" << std::endl;

    ww4_std_out::extcde(1, std::cerr, "Missing or invalid mandatory fields.",
                        __FILE__, __LINE__);
  }

  // Time step validation
  if (config.timeStep <= 0.0) {
    std::cerr << "WW4 ERROR: Mandatory time step missing or invalid "
                 "in configuration."
              << std::endl;
    ww4_std_out::extcde(1, std::cerr, "Missing or invalid mandatory time step.",
                        __FILE__, __LINE__);
  }

  // Output validation
  bool outputValid = true;
  auto validateOutput = [&](const OutputConfig &oc, std::string_view name) {
    if (oc.requested && oc.interval <= 0.0) {
      std::cerr << "WW4 ERROR: Mandatory interval missing or invalid for "
                   "requested output: "
                << name << std::endl;
      return false;
    }
    return true;
  };

  outputValid &= validateOutput(config.outputFields, "output_fields");
  outputValid &= validateOutput(config.outputPoints, "output_points");
  outputValid &= validateOutput(config.outputNesting, "output_nesting");
  outputValid &= validateOutput(config.outputTracks, "output_tracks");
  outputValid &= validateOutput(config.outputRestart, "output_restart");

  if (!outputValid) {
    ww4_std_out::extcde(1, std::cerr,
                        "Missing or invalid mandatory output interval(s).",
                        __FILE__, __LINE__);
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

  os << "     Calendar type        : " << calType << std::endl;
  os << "     Screen output        : " << (config.produceStdOut ? "yes" : "no")
     << std::endl;
  os << "     Log file             : " << (config.produceLogFile ? "yes" : "no")
     << std::endl;

  const bool isConventional = !config.dryRun && config.propagateX &&
                              config.propagateY && config.propagateTheta &&
                              config.propagateK && config.sourceTerms;

  os << "     Conventional model run"
     << std::endl;

  if (!isConventional) {
    os << "     Unconventional model run"

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

  os << "     Water levels       : " << inputOptionToString(config.waterLevels)
     << std::endl;
  os << "     Currents           : " << inputOptionToString(config.currents)
     << std::endl;
  os << "     Winds              : " << inputOptionToString(config.winds)
     << std::endl;
  os << "     Ice concentrations : "
     << inputOptionToString(config.iceConcentrations) << std::endl;
  os << "     Bottom depth       : " << inputOptionToString(config.bottomDepth)
     << std::endl;

  os << "     Time step          : " << config.timeStep << " s" << std::endl;

  reportOutput(config.outputFields, "Gridded fields", os);
  reportOutput(config.outputPoints, "Point", os);
  reportOutput(config.outputNesting, "Nesting data", os);
  reportOutput(config.outputTracks, "Track", os);
  reportOutput(config.outputRestart, "Restart file", os);

  os << std::endl;
}

} // namespace ww4_utils
