/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_standalone_config.h
 * @brief Service routine for processing stand-alone configuration YAML file.
 * @details This header defines the StandaloneConfig structure and the
 *          loadStandaloneConfig function, which reads and validates the
 *          configuration for the stand-alone program.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-02
 * @date Last update : 2026-05-20
 */

#pragma once

#include "ww4_utils/time_management.h"
#include <iostream>
#include <optional>
#include <string_view>

namespace ww4_utils {

struct StandaloneConfig {
  DateTime startTime;
  DateTime endTime;
};

std::optional<DateTime> parseDateTimeString(std::string_view s);

std::optional<StandaloneConfig> loadStandaloneConfig(std::string_view filename,
                                                     std::ostream &os) noexcept;

void reportStandaloneConfig(const StandaloneConfig &config, std::ostream &os);

} // namespace ww4_utils
