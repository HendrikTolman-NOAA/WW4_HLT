/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_output_utils.h
 * @brief Utility structures and routines for model output processing.
 * @details This header defines the OutputConfig structure and routines for
 *          managing model output time data.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */

#pragma once

#include "ww4_utils/time_management.h"
#include <optional>

namespace ww4_utils {

// Forward declaration of RunConfig to avoid circular dependency
struct RunConfig;

/**
 * @struct OutputConfig
 * @brief Configuration for a specific output type.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
struct OutputConfig {
  bool requested = false;             ///< Is this output requested?
  std::optional<DateTime> startTime;  ///< Start time for output.
  std::optional<DateTime> endTime;    ///< End time for output.
  double interval = -1.0;             ///< Output interval in seconds.
  bool atFirstTime = true;            ///< Output at first time?
  std::optional<DateTime> actualTime; ///< Next scheduled output time.
};

void assessOutputConfig(const DateTime &modelTime, const DateTime &endTime,
                        RunConfig &config);

double computeOutputTimeStep(const DateTime &modelTime, const DateTime &endTime,
                             const RunConfig &config);

void updateOutputActualTimes(const DateTime &modelTime, const DateTime &endTime,
                             RunConfig &config);

} // namespace ww4_utils
