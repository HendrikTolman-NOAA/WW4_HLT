/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_stand_alone_config.hpp
 * @brief Service routine for processing stand-alone configuration YAML file.
 * @details This header defines the StandAloneConfig structure and the
 *          loadStandAloneConfig function, which reads and validates the
 *          configuration for the stand-alone program.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-02
 * @date Last Update, 2026-04-09
 */

#ifndef WW4_UTILS_WW4_STAND_ALONE_CONFIG_HPP
#define WW4_UTILS_WW4_STAND_ALONE_CONFIG_HPP

#include "ww4_utils/time_management.hpp"
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

/**
 * @brief Internal helper to parse a date-time string in "YYYYMMDD HHMMSS"
 * format.
 * @param s The string view to parse.
 * @return A DateTime structure if successful, or std::nullopt.
 */
std::optional<DateTime> parseDateTimeString(std::string_view s);

/**
 * @brief Loads the stand-alone configuration from a YAML file.
 * @details Reads the specified YAML file from the current directory,
 *          extracts the start and end times, and validates that the
 *          end time is not before the start time.
 * @param filename The name of the YAML file to load.
 * @return A StandAloneConfig structure if successful, or std::nullopt
 *         if an error occurred (e.g., file not found, invalid format,
 *         or validation failure).
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
std::optional<StandAloneConfig>
loadStandAloneConfig(std::string_view filename) noexcept;

} // namespace ww4_utils

#endif // WW4_UTILS_WW4_STAND_ALONE_CONFIG_HPP
