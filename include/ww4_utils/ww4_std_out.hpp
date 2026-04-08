/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_std_out.hpp
 * @brief Routines for standard screen output.
 * @details This header defines routines for managing screen output to std_out,
 *          duplicating the formats from WAVEWATCH III, updated for
 *          WAVEWATCH IV.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last Update, 2026-04-06
 * @note Converted from WAVEWATCH III (ww3_shel.F90, ww3_multi.F90, and
 *       w3servmd.F90).
 *       Original author: Hendrik L. Tolman.
 */

#ifndef WW4_UTILS_WW4_STD_OUT_HPP
#define WW4_UTILS_WW4_STD_OUT_HPP

#include "ww4_utils/memory_utils.hpp"
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

/**
 * @namespace ww4_utils
 * @brief Utilities for WAVEWATCH IV.
 */
namespace ww4_utils {

/**
 * @namespace ww4_std_out
 * @brief Routines for standard screen output.
 */
namespace ww4_std_out {

/**
 * @brief Writes the initial banner to the provided output stream.
 * @details Duplicates the initial Fortran FORMAT statements from WW3.
 *          Identified as format 900 in ww3_shel.F90 and ww3_multi.F90.
 * @param os The output stream to write to (e.g., std::cout).
 * @param programName The name of the executable program to identify in the
 *        banner.
 */
void writeInitialOutput(std::ostream &os, const std::string &programName);

/**
 * @brief Writes the final footer to the provided output stream.
 * @details Duplicates the final Fortran FORMAT statements from WW3.
 *          Identified as format 999 in ww3_shel.F90 and ww3_multi.F90.
 *          Optionally includes execution times and memory usage.
 * @param os The output stream to write to (e.g., std::cout).
 * @param programName The name of the executable program to identify in the
 *        footer.
 * @param initTime Optional initialization time in seconds.
 * @param elapsedTotal Optional total elapsed time in seconds.
 * @param memory Optional memory usage metrics.
 */
void writeFinalOutput(std::ostream &os, const std::string &programName,
                      std::optional<double> initTime = std::nullopt,
                      std::optional<double> elapsedTotal = std::nullopt,
                      std::optional<MemoryUsage> memory = std::nullopt);

/**
 * @brief Writes an error message to the provided output stream in the standard
 *        WAVEWATCH format.
 * @details Heritage from EXTCDE in WAVEWATCH III w3servmd.F90.
 * @param os The output stream to write to.
 * @param msg Optional error message to report.
 * @param file Optional source file name where the error occurred.
 * @param line Optional line number in the source file.
 */
void writeExtcdeOutput(std::ostream &os,
                       std::optional<std::string_view> msg = std::nullopt,
                       std::optional<std::string_view> file = std::nullopt,
                       std::optional<int> line = std::nullopt);

/**
 * @brief Performs a program stop with an exit code.
 * @details Heritage from EXTCDE in WAVEWATCH III w3servmd.F90.
 *          Calls writeExtcdeOutput and then std::exit.
 * @param exitCode The exit code to return to the environment.
 * @param os The output stream to write to (defaults to std::cerr).
 * @param msg Optional error message to report.
 * @param file Optional source file name where the error occurred.
 * @param line Optional line number in the source file.
 */
[[noreturn]] void extcde(int exitCode, std::ostream &os = std::cerr,
                         std::optional<std::string_view> msg = std::nullopt,
                         std::optional<std::string_view> file = std::nullopt,
                         std::optional<int> line = std::nullopt);

} // namespace ww4_std_out
} // namespace ww4_utils

#endif // WW4_UTILS_WW4_STD_OUT_HPP
