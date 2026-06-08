/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_run_config.h
 * @brief Service routine for processing run-time configuration YAML file.
 * @details This header defines the RunConfig structure and the
 *          loadRunConfig function, which reads and processes the
 *          run-time configuration for WAVEWATCH IV.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last update : 2026-06-08
 */

#pragma once

#include "ww4_utils/time_management.h"
#include "ww4_utils/ww4_output_utils.h"
#include <iostream>
#include <optional>
#include <string_view>
#include <vector>

namespace ww4_utils {

enum class InputFieldOption {
  Undefined,
  None,
  FromFile,
  FromCoupling,
  Homogeneous,
  FromGrid
};

enum class ScreenOutputLevel { None, Summary, Full };

enum class EchoOption { None, Summary, Full };

struct HomogeneousDataPoint {
  DateTime time;
  std::vector<double> values;
};

struct RunConfig {
  TimeManagement::CalendarType calendarType =
      TimeManagement::CalendarType::Standard;
  bool produceStdOut = true;
  bool produceLogFile = true;
  bool dryRun = false;
  bool propagateX = true;
  bool propagateY = true;
  bool propagateTheta = true;
  bool propagateK = true;
  bool sourceTerms = true;

  InputFieldOption waterLevels = InputFieldOption::Undefined;
  InputFieldOption currents = InputFieldOption::Undefined;
  InputFieldOption winds = InputFieldOption::Undefined;
  InputFieldOption iceConcentrations = InputFieldOption::Undefined;
  InputFieldOption bottomDepth = InputFieldOption::FromGrid;

  EchoOption echoHomInput = EchoOption::Summary;

  ScreenOutputLevel screenOutputLevel = ScreenOutputLevel::Summary;

  std::vector<HomogeneousDataPoint> homogeneousWaterLevels;
  std::vector<HomogeneousDataPoint> homogeneousCurrents;
  std::vector<HomogeneousDataPoint> homogeneousWinds;
  std::vector<HomogeneousDataPoint> homogeneousIceConcentrations;
  std::vector<HomogeneousDataPoint> homogeneousBottomDepth;

  double timeStep = -1.0;

  OutputConfig outputApi;

  OutputConfig outputFields;
  OutputConfig outputPoints;
  OutputConfig outputRestart;
};

std::string_view cleanValue(std::string_view s);

std::optional<RunConfig> loadRunConfig(std::string_view filename,
                                       std::ostream &os) noexcept;

void reportRunConfig(const RunConfig &config, std::ostream &os);

} // namespace ww4_utils
