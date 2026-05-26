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
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last update : 2026-05-26
 */

#include "ww4_utils/ww4_run_config.h"
#include "ww4_utils/ww4_standalone_config.h"
#include "ww4_utils/ww4_std_out.h"
#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <string>

namespace ww4_utils {

/**
 * @enum InputFieldOption
 * @brief Options for model input fields.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @var InputFieldOption::Undefined
 * @brief Mandatory field not yet defined.
 * @var InputFieldOption::None
 * @brief No input, data set to zero.
 * @var InputFieldOption::FromFile
 * @brief Input from file.
 * @var InputFieldOption::FromCoupling
 * @brief Input from coupling.
 * @var InputFieldOption::Homogeneous
 * @brief Homogeneous field.
 * @var InputFieldOption::FromGrid
 * @brief Static data read as part of the grid data.
 */

/**
 * @enum ScreenOutputLevel
 * @brief Options for the level of output to standard output during time
 * stepping.
 * @author Main Author(s): Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @var ScreenOutputLevel::None
 * @brief No output in the time stepping loop.
 * @var ScreenOutputLevel::Summary
 * @brief Output if input field updated or output produced.
 * @var ScreenOutputLevel::Full
 * @brief Standard full output for every step.
 */

/**
 * @enum EchoOption
 * @brief Options for echoing input data to standard output and log files.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @var EchoOption::None
 * @brief No echoing of input data.
 * @var EchoOption::Summary
 * @brief Produce a summary of the input data.
 * @var EchoOption::Full
 * @brief Full reproduction of the user input data.
 */

/**
 * @struct HomogeneousDataPoint
 * @brief Data point for a homogeneous input field.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @var HomogeneousDataPoint::time
 * @brief Time of the data point.
 * @var HomogeneousDataPoint::values
 * @brief Values of the data point.
 */

/**
 * @struct RunConfig
 * @brief Configuration for the run-time environment.
 * @details Stores the calendar type, output preferences, and input options.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @var RunConfig::calendarType
 * @brief Calendar type.
 * @var RunConfig::produceStdOut
 * @brief Screen output flag.
 * @var RunConfig::produceLogFile
 * @brief Log file flag.
 * @var RunConfig::dryRun
 * @brief Dry run flag.
 * @var RunConfig::propagateX
 * @brief Propagation flag in x-direction.
 * @var RunConfig::propagateY
 * @brief Propagation flag in y-direction.
 * @var RunConfig::propagateTheta
 * @brief Propagation flag in theta-direction.
 * @var RunConfig::propagateK
 * @brief Propagation flag in k-direction.
 * @var RunConfig::sourceTerms
 * @brief Source terms flag.
 * @var RunConfig::waterLevels
 * @brief Water levels option.
 * @var RunConfig::currents
 * @brief Currents option.
 * @var RunConfig::winds
 * @brief Winds option.
 * @var RunConfig::iceConcentrations
 * @brief Ice concentrations option.
 * @var RunConfig::bottomDepth
 * @brief Bottom depth option.
 * @var RunConfig::echoHomInput
 * @brief Echo input flag.
 * @var RunConfig::screenOutputLevel
 * @brief Screen output level.
 * @var RunConfig::homogeneousWaterLevels
 * @brief Homogeneous water levels data.
 * @var RunConfig::homogeneousCurrents
 * @brief Homogeneous currents data.
 * @var RunConfig::homogeneousWinds
 * @brief Homogeneous winds data.
 * @var RunConfig::homogeneousIceConcentrations
 * @brief Homogeneous ice concentrations data.
 * @var RunConfig::homogeneousBottomDepth
 * @brief Homogeneous bottom depth data.
 * @var RunConfig::timeStep
 * @brief Model time step in seconds.
 * @var RunConfig::outputApi
 * @brief API output configuration.
 * @var RunConfig::outputFields
 * @brief Gridded fields output configuration.
 * @var RunConfig::outputPoints
 * @brief Point output configuration.
 * @var RunConfig::outputRestart
 * @brief Restart file output configuration.
 */

namespace {

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
  const auto dt = parseDateTimeString(dateTimePart);
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
 * @brief Helper to parse InputFieldOption from string.
 * @param value The string value to parse.
 * @param allowFromGrid Whether to allow the 'from_grid' option.
 * @return The corresponding InputFieldOption.
 */
InputFieldOption parseInputOption(const std::string_view value,
                                  const bool allowFromGrid = false) {
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
std::string inputOptionToString(const InputFieldOption option) {
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
void updateOutputConfig(OutputConfig &oc, const std::string_view key_suffix,
                        const std::string_view value) {
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
 * @brief Helper to echo a homogeneous data series.
 * @param processed Vector of data points.
 * @param fieldName Name of the field.
 * @param option Echo level.
 * @param os Output stream.
 */
void echoHomogeneousData(const std::vector<HomogeneousDataPoint> &processed,
                         std::string_view /*fieldName*/, EchoOption option,
                         std::ostream &os) {
  if (processed.empty() || option == EchoOption::None)
    return;

  if (option == EchoOption::Summary) {
    os << "        Number of data points: " << processed.size() << std::endl;
  } else if (option == EchoOption::Full) {
    for (const auto &dp : processed) {
      os << "        " << TimeManagement::toFormattedString(dp.time) << " :";
      for (const auto val : dp.values) {
        os << " " << val;
      }
      os << std::endl;
    }
  }
}

/**
 * @brief Helper to report OutputConfig settings.
 * @param oc The OutputConfig structure to report.
 * @param label The label for the output type.
 * @param os The output stream to write to.
 */
void reportOutput(const OutputConfig &oc, const std::string_view label,
                  std::ostream &os) {
  if (oc.requested) {
    os << "\n     " << label << " output" << std::endl;
    if (label != "API") {
      os << "        Interval          : " << oc.interval << " s" << std::endl;
      os << "        At first time     : " << (oc.atFirstTime ? "yes" : "no")
         << std::endl;
    }
    if (oc.startTime) {
      os << "        Start time        : "
         << TimeManagement::toFormattedString(*oc.startTime) << std::endl;
    }
    if (oc.endTime) {
      os << "        End time          : "
         << TimeManagement::toFormattedString(*oc.endTime) << std::endl;
    }
  } else {
    os << "\n     " << label << " output not requested " << std::endl;
  }
}

} // namespace

/**
 * @brief Internal helper to trim whitespace and quotes from a string.
 * @param s The string view to clean.
 * @return A cleaned string view.
 */
std::string_view cleanValue(const std::string_view s) {
  const size_t start = s.find_first_not_of(" \t\r\n\"");
  const size_t end = s.find_last_not_of(" \t\r\n\"");
  if (start == std::string_view::npos)
    return "";
  return s.substr(start, end - start + 1);
}

/**
 * @brief Loads the run-time configuration from a YAML file.
 * @details Reads the specified YAML file from the current directory,
 *          extracts the configuration settings, and updates the
 *          TimeManagement calendar type.
 * @param filename The name of the YAML file to load.
 * @param os Output stream for reporting.
 * @return A RunConfig structure containing the loaded (or default) settings,
 *         or std::nullopt if the file could not be opened.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
std::optional<RunConfig> loadRunConfig(const std::string_view filename,
                                       std::ostream &os) noexcept {
  std::ifstream file((std::string(filename)));
  RunConfig config{};

  if (!file.is_open()) {
    return std::nullopt;
  }

  std::string lastKey = "";
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
    if (first == std::string_view::npos)
      continue;

    if (lineView[first] == '-') {
      const std::string_view value = cleanValue(lineView.substr(first + 1));
      if (!value.empty()) {
        auto dp = parseHomogeneousString(value);
        if (dp) {
          if (lastKey == "water_levels") {
            config.homogeneousWaterLevels.push_back(*dp);
          } else if (lastKey == "currents") {
            config.homogeneousCurrents.push_back(*dp);
          } else if (lastKey == "winds") {
            config.homogeneousWinds.push_back(*dp);
          } else if (lastKey == "ice_concentrations") {
            config.homogeneousIceConcentrations.push_back(*dp);
          } else if (lastKey == "bottom_depth") {
            config.homogeneousBottomDepth.push_back(*dp);
          }
        }
      }
      continue;
    }

    const size_t colonPos = lineView.find(':');
    if (colonPos == std::string_view::npos)
      continue;

    const std::string_view key_raw = lineView.substr(first, colonPos - first);
    // Trim trailing whitespace from key
    const size_t kend = key_raw.find_last_not_of(" \t");
    const std::string_view key = (kend != std::string_view::npos)
                                     ? key_raw.substr(0, kend + 1)
                                     : key_raw;
    lastKey = std::string(key);

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
    } else if (key == "echo_hom_input") {
      if (value == "none") {
        config.echoHomInput = EchoOption::None;
      } else if (value == "summary") {
        config.echoHomInput = EchoOption::Summary;
      } else if (value == "full") {
        config.echoHomInput = EchoOption::Full;
      }
    } else if (key == "screen_output_level") {
      if (value == "none") {
        config.screenOutputLevel = ScreenOutputLevel::None;
      } else if (value == "summary") {
        config.screenOutputLevel = ScreenOutputLevel::Summary;
      } else if (value == "full") {
        config.screenOutputLevel = ScreenOutputLevel::Full;
      }
    } else if (key == "time_step") {
      if (std::from_chars(value.data(), value.data() + value.size(),
                          config.timeStep)
              .ec != std::errc()) {
        config.timeStep = -1.0;
      }
    } else if (key == "output_api") {
      config.outputApi.requested = (value == "yes");
    } else if (key.starts_with("output_api_")) {
      updateOutputConfig(config.outputApi, key.substr(11), value);
    } else if (key.starts_with("output_fields_")) {
      updateOutputConfig(config.outputFields, key.substr(14), value);
    } else if (key.starts_with("output_points_")) {
      updateOutputConfig(config.outputPoints, key.substr(14), value);
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
    os << "WW4 ERROR: Mandatory model input field(s) missing or invalid "
          "in configuration."
       << std::endl;
    if (config.waterLevels == InputFieldOption::Undefined)
      os << "   Missing/invalid: water_levels" << std::endl;
    if (config.currents == InputFieldOption::Undefined)
      os << "   Missing/invalid: currents" << std::endl;
    if (config.winds == InputFieldOption::Undefined)
      os << "   Missing/invalid: winds" << std::endl;
    if (config.iceConcentrations == InputFieldOption::Undefined)
      os << "   Missing/invalid: ice_concentrations" << std::endl;
    if (config.bottomDepth == InputFieldOption::Undefined)
      os << "   Missing/invalid: bottom_depth" << std::endl;

    ww4_std_out::extcde(1, os, "Missing or invalid mandatory fields.", __FILE__,
                        __LINE__);
  }

  // Time step validation
  if (config.timeStep < 0.0) {
    os << "WW4 ERROR: Mandatory time step missing or invalid "
          "in configuration."
       << std::endl;
    ww4_std_out::extcde(1, os, "Missing or invalid mandatory time step.",
                        __FILE__, __LINE__);
  }

  // Output validation
  bool outputValid = true;
  auto validateOutput = [&](const OutputConfig &oc,
                            const std::string_view name) {
    if (oc.requested && name != "output_api" && oc.interval <= 0.0) {
      os << "WW4 ERROR: Mandatory interval missing or invalid for "
            "requested output: "
         << name << std::endl;
      return false;
    }
    return true;
  };

  outputValid &= validateOutput(config.outputFields, "output_fields");
  outputValid &= validateOutput(config.outputApi, "output_api");
  outputValid &= validateOutput(config.outputPoints, "output_points");
  outputValid &= validateOutput(config.outputRestart, "output_restart");

  if (!outputValid) {
    ww4_std_out::extcde(1, os,
                        "Missing or invalid mandatory output interval(s).",
                        __FILE__, __LINE__);
  }

  // Update TimeManagement with the loaded calendar type.
  TimeManagement::setCalendarType(config.calendarType);

  return config;
}

/**
 * @brief Reports the current configuration to the provided output stream.
 * @param config The RunConfig structure to report.
 * @param os The output stream to write to (default: std::cout).
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
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

  std::string echoStr = "summary";
  if (config.echoHomInput == EchoOption::None) {
    echoStr = "none";
  } else if (config.echoHomInput == EchoOption::Full) {
    echoStr = "full";
  }
  os << "     Echo input           : " << echoStr << std::endl;

  std::string screenStr = "full";
  if (config.screenOutputLevel == ScreenOutputLevel::None) {
    screenStr = "none";
  } else if (config.screenOutputLevel == ScreenOutputLevel::Summary) {
    screenStr = "summary";
  }
  os << "     Screen output level  : " << screenStr << std::endl;

  const bool isConventional = !config.dryRun && config.propagateX &&
                              config.propagateY && config.propagateTheta &&
                              config.propagateK && config.sourceTerms;

  if (isConventional) {
    os << "     Conventional model run" << std::endl;
  } else {
    os << "     Unconventional model run" << std::endl;
    if (config.dryRun) {
      os << "        Dry run" << std::endl;
    } else {
      os << "        Propagate X       : " << (config.propagateX ? "yes" : "no")
         << std::endl;
      os << "        Propagate Y       : " << (config.propagateY ? "yes" : "no")
         << std::endl;
      os << "        Propagate Theta   : "
         << (config.propagateTheta ? "yes" : "no") << std::endl;
      os << "        Propagate K       : " << (config.propagateK ? "yes" : "no")
         << std::endl;
      os << "        Source terms      : "
         << (config.sourceTerms ? "yes" : "no") << std::endl;
    }
  }

  os << "     Time step            : " << config.timeStep << " s" << std::endl;

  os << "\n  Model input:" << std::endl;

  os << "     Bottom depth         : "
     << inputOptionToString(config.bottomDepth) << std::endl;
  if (config.bottomDepth == InputFieldOption::Homogeneous) {
    echoHomogeneousData(config.homogeneousBottomDepth, "bottom depth",
                        config.echoHomInput, os);
  }
  os << "     Water levels         : "
     << inputOptionToString(config.waterLevels) << std::endl;
  if (config.waterLevels == InputFieldOption::Homogeneous) {
    echoHomogeneousData(config.homogeneousWaterLevels, "water levels",
                        config.echoHomInput, os);
  }
  os << "     Currents             : " << inputOptionToString(config.currents)
     << std::endl;
  if (config.currents == InputFieldOption::Homogeneous) {
    echoHomogeneousData(config.homogeneousCurrents, "currents",
                        config.echoHomInput, os);
  }
  os << "     Winds                : " << inputOptionToString(config.winds)
     << std::endl;
  if (config.winds == InputFieldOption::Homogeneous) {
    echoHomogeneousData(config.homogeneousWinds, "winds", config.echoHomInput,
                        os);
  }
  os << "     Ice concentrations   : "
     << inputOptionToString(config.iceConcentrations) << std::endl;
  if (config.iceConcentrations == InputFieldOption::Homogeneous) {
    echoHomogeneousData(config.homogeneousIceConcentrations,
                        "ice concentrations", config.echoHomInput, os);
  }

  os << "\n  Model output:" << std::endl;

  reportOutput(config.outputFields, "Gridded fields", os);
  reportOutput(config.outputPoints, "Point", os);
  reportOutput(config.outputRestart, "Restart file", os);
  reportOutput(config.outputApi, "API", os);

  os << std::endl;
}

} // namespace ww4_utils
