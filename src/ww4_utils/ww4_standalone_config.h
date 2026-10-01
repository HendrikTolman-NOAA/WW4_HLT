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
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kit Stokes, Jessica Meixner
 * @date Initial, 2026-04-02
 * @date Last update : 2026-09-28
 */

#pragma once

#include "ww4_utils/time_management.h"
#include <iostream>
#include <optional>
#include <string_view>

namespace ww4_utils {

// --- StandaloneConfig -------------------------------------------------------
/**
 * @struct StandaloneConfig
 * @brief Configuration for the ww4_standalone program.
 * @details Stores the start and end times for the simulation.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI), Kit Stokes, Jessica Meixner
 * @date Initial, 2026-04-02
 * @date Last update : 2026-09-28
 * @var StandaloneConfig::startTime
 * @brief Simulation start time.
 * @var StandaloneConfig::endTime
 * @brief Simulation end time.
 */
struct StandaloneConfig {
  DateTime startTime;
  DateTime endTime;
};

// --- parseDateTimeString ----------------------------------------------------
/**
 * @brief Helper to parse a date-time string in "YYYYMMDD HHMMSS" format.
 * @param s The string view to parse.
 * @return A DateTime structure if successful, or std::nullopt.
 */
std::optional<DateTime> parseDateTimeString(std::string_view s);

// --- loadStandaloneConfig ---------------------------------------------------
/**
 * @brief Loads the stand-alone configuration from a YAML file.
 * @param filename The name of the YAML file to load.
 * @param os Output stream for reporting.
 * @return A StandaloneConfig structure if successful, or std::nullopt.
 */
std::optional<StandaloneConfig> loadStandaloneConfig(std::string_view filename,
                                                     std::ostream &os) noexcept;

// --- reportStandaloneConfig -------------------------------------------------
/**
 * @brief Reports the stand-alone configuration to the provided output stream.
 * @param config The StandaloneConfig structure to report.
 * @param os The output stream to write to.
 */
void reportStandaloneConfig(const StandaloneConfig &config, std::ostream &os);

} // namespace ww4_utils
