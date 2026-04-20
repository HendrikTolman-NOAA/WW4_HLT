/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_logfile.hpp
 * @brief Routines for log file output.
 * @details This header defines routines for managing log file output,
 *          duplicating the formats from WAVEWATCH III, updated for
 *          WAVEWATCH IV.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last update, 2026-04-20
 * @note Converted from WAVEWATCH III (ww3_shel.F90 and ww3_multi.F90).
 *       Original author: Hendrik L. Tolman.
 */

#ifndef WW4_UTILS_WW4_LOGFILE_HPP
#define WW4_UTILS_WW4_LOGFILE_HPP

#include "ww4_utils/memory_utils.hpp"
#include <iostream>
#include <optional>
#include <string>

/**
 * @namespace ww4_utils
 * @brief Utilities for WAVEWATCH IV.
 */
namespace ww4_utils {

/**
 * @namespace ww4_logfile
 * @brief Routines for log file output.
 */
namespace ww4_logfile {

/**
 * @brief Writes the initial log entry to the provided output stream.
 * @details Duplicates the initial log formatting from WW3.
 * @param os The output stream to write to (e.g., an std::ofstream).
 * @param programName The name of the executable program.
 */
void writeInitialOutput(std::ostream &os, std::string_view programName);

/**
 * @brief Writes the final log entry to the provided output stream.
 * @details Duplicates the final log formatting from WW3.
 *          Optionally includes execution times and memory usage.
 * @param os The output stream to write to.
 * @param programName The name of the executable program.
 * @param initTime Optional initialization time in seconds.
 * @param elapsedTotal Optional total elapsed time in seconds.
 */
void writeFinalOutput(std::ostream &os, std::string_view programName,
                      std::optional<double> initTime = std::nullopt,
                      std::optional<double> elapsedTotal = std::nullopt);

} // namespace ww4_logfile
} // namespace ww4_utils

#endif // WW4_UTILS_WW4_LOGFILE_HPP
