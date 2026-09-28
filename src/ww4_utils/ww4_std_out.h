/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_std_out.h
 * @brief Routines for standard screen output.
 * @details This header defines routines for managing screen output to std_out,
 *          duplicating the formats from WAVEWATCH III, updated for
 *          WAVEWATCH IV.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last update : 2026-09-24
 * @note Converted from WAVEWATCH III (ww3_shel.F90, ww3_multi.F90, and
 *       w3servmd.F90).
 *       Original author: Hendrik L. Tolman.
 */

#pragma once

#include "ww4_utils/memory_utils.h"
#include "ww4_utils/time_management.h"
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace ww4_utils {

namespace ww4_std_out {

// --- writeInitialOutput -----------------------------------------------------
/**
 * @brief Writes the initial banner to the provided output stream.
 * @param os The output stream to write to.
 * @param programName Name of the executable program.
 */
void writeInitialOutput(std::ostream &os, std::string_view programName);

// --- writeFinalOutput -------------------------------------------------------
/**
 * @brief Writes the final footer to the provided output stream.
 * @param os The output stream to write to.
 * @param programName Name of the executable program.
 * @param initTime Optional initialization time in seconds.
 * @param elapsedTotal Optional total elapsed time in seconds.
 */
void writeFinalOutput(std::ostream &os, std::string_view programName,
                      std::optional<double> initTime = std::nullopt,
                      std::optional<double> elapsedTotal = std::nullopt);

// --- writeExtcdeOutput ------------------------------------------------------
/**
 * @brief Writes an error message to the provided output stream in standard
 * format.
 * @param os The output stream to write to.
 * @param msg Optional error message to report.
 * @param file Optional source file name where the error occurred.
 * @param line Optional line number in the source file.
 */
void writeExtcdeOutput(std::ostream &os,
                       std::optional<std::string_view> msg = std::nullopt,
                       std::optional<std::string_view> file = std::nullopt,
                       std::optional<int> line = std::nullopt);

// --- writeWarnngOutput ------------------------------------------------------
/**
 * @brief Writes a warning message to the provided output stream in standard
 * format.
 * @param os The output stream to write to.
 * @param msg Warning message to report.
 * @param file Optional source file name where warning occurred.
 * @param line Optional line number in source file.
 */
void writeWarnngOutput(std::ostream &os, std::string_view msg,
                       std::optional<std::string_view> file = std::nullopt,
                       std::optional<int> line = std::nullopt);

// --- extcde -----------------------------------------------------------------
/**
 * @brief Performs a program stop with an exit code.
 * @param exitCode Exit code to return to environment.
 * @param os Output stream to write to.
 * @param msg Optional error message.
 * @param file Optional source file name.
 * @param line Optional line number.
 */
[[noreturn]] void extcde(int exitCode, std::ostream &os,
                         std::optional<std::string_view> msg = std::nullopt,
                         std::optional<std::string_view> file = std::nullopt,
                         std::optional<int> line = std::nullopt);

// --- warnng -----------------------------------------------------------------
/**
 * @brief Reports a warning and continues execution.
 * @param os Output stream to write to.
 * @param msg Warning message to report.
 * @param file Optional source file name.
 * @param line Optional line number.
 */
void warnng(std::ostream &os, std::string_view msg,
            std::optional<std::string_view> file = std::nullopt,
            std::optional<int> line = std::nullopt);

// --- writeUpdatingField -----------------------------------------------------
/**
 * @brief Writes a message identifying that an input field is being updated.
 * @param os Output stream to write to.
 * @param fieldName Name of field being updated.
 */
void writeUpdatingField(std::ostream &os, std::string_view fieldName);

// --- writeInterpolationInfo -------------------------------------------------
/**
 * @brief Writes interpolation interval information for an input field.
 * @param os Output stream to write to.
 * @param time1 First interpolation time tag.
 * @param time2 Second interpolation time tag.
 */
void writeInterpolationInfo(std::ostream &os, const DateTime &time1,
                            const DateTime &time2);

} // namespace ww4_std_out
} // namespace ww4_utils
