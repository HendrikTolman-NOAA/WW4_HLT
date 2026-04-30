/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_stand_alone_config.h
 * @brief Service routine for processing stand-alone configuration YAML file.
 * @details This header defines the StandAloneConfig structure and the
 *          loadStandAloneConfig function, which reads and validates the
 *          configuration for the stand-alone program.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-02
 * @date Last update : 2026-04-30
 */

#pragma once

#include "ww4_utils/time_management.h"
#include <iostream>
#include <optional>
#include <string_view>

namespace ww4_utils {

/**
 * @struct StandAloneConfig
 * @brief Configuration for the ww4_stand_alone program.
 * @details Stores the start and end times for the simulation.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
struct StandAloneConfig {
  DateTime startTime; ///< Simulation start time.
  DateTime endTime;   ///< Simulation end time.
};

std::optional<DateTime> parseDateTimeString(std::string_view s);

std::optional<StandAloneConfig> loadStandAloneConfig(std::string_view filename,
                                                     std::ostream &os) noexcept;

void reportStandAloneConfig(const StandAloneConfig &config, std::ostream &os);

} // namespace ww4_utils
