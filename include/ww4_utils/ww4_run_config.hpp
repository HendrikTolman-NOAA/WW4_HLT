/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_run_config.hpp
 * @brief Service routine for processing run-time configuration YAML file.
 * @details This header defines the RunConfig structure and the
 *          loadRunConfig function, which reads and processes the
 *          run-time configuration for WAVEWATCH IV.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last Update, 2026-04-13
 */

#ifndef WW4_UTILS_WW4_RUN_CONFIG_HPP
#define WW4_UTILS_WW4_RUN_CONFIG_HPP

#include "ww4_utils/time_management.hpp"
#include <iostream>
#include <optional>
#include <string_view>

namespace ww4_utils {

/**
 * @struct RunConfig
 * @brief Configuration for the run-time environment.
 * @details Stores the calendar type and output preferences.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
struct RunConfig {
  TimeManagement::CalendarType calendarType =
      TimeManagement::CalendarType::Standard; ///< Calendar type.
  bool produceStdOut = true;                  ///< Screen output flag.
  bool produceLogFile = true;                 ///< Log file flag.
  bool dryRun = false;                        ///< Dry run flag.
  bool propagateX = true;     ///< Propagation flag in x-direction.
  bool propagateY = true;     ///< Propagation flag in y-direction.
  bool propagateTheta = true; ///< Propagation flag in theta-direction.
  bool propagateK = true;     ///< Propagation flag in k-direction.
  bool sourceTerms = true;    ///< Source terms flag.
};

/**
 * @brief Internal helper to trim whitespace and quotes from a string.
 * @param s The string view to clean.
 * @return A cleaned string view.
 */
std::string_view cleanValue(std::string_view s);

/**
 * @brief Loads the run-time configuration from a YAML file.
 * @details Reads the specified YAML file from the current directory,
 *          extracts the configuration settings, and updates the
 *          TimeManagement calendar type.
 * @param filename The name of the YAML file to load.
 * @return A RunConfig structure containing the loaded (or default) settings,
 *         or std::nullopt if the file could not be opened.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
std::optional<RunConfig> loadRunConfig(std::string_view filename) noexcept;

/**
 * @brief Reports the current configuration to the provided output stream.
 * @param config The RunConfig structure to report.
 * @param os The output stream to write to (default: std::cout).
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-07
 */
void reportRunConfig(const RunConfig &config, std::ostream &os = std::cout);

} // namespace ww4_utils

#endif // WW4_UTILS_WW4_RUN_CONFIG_HPP
