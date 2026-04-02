/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file run_config.hpp
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
 * @date Last Update, 2026-04-03
 */

#ifndef WW4_UTILS_RUN_CONFIG_HPP
#define WW4_UTILS_RUN_CONFIG_HPP

#include "ww4_utils/time_management.hpp"
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
};

/**
 * @brief Loads the run-time configuration from a YAML file.
 * @details Reads the specified YAML file from the current directory,
 *          extracts the configuration settings, and updates the
 *          TimeManagement calendar type.
 * @param filename The name of the YAML file to load.
 * @return A RunConfig structure containing the loaded (or default) settings.
 * @author Main Author(s): Hendrik L. Tolman, Aldgisl (AI Persona)
 * @author Contributors: Jules (Agentic AI)
 */
RunConfig loadRunConfig(std::string_view filename) noexcept;

} // namespace ww4_utils

#endif // WW4_UTILS_RUN_CONFIG_HPP
