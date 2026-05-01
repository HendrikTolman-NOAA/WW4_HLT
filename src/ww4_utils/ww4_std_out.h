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
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-01
 * @date Last update : 2026-05-01
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

void writeInitialOutput(std::ostream &os, std::string_view programName);

void writeFinalOutput(std::ostream &os, std::string_view programName,
                      std::optional<double> initTime = std::nullopt,
                      std::optional<double> elapsedTotal = std::nullopt);

void writeExtcdeOutput(std::ostream &os,
                       std::optional<std::string_view> msg = std::nullopt,
                       std::optional<std::string_view> file = std::nullopt,
                       std::optional<int> line = std::nullopt);

void writeWarnngOutput(std::ostream &os, std::string_view msg,
                       std::optional<std::string_view> file = std::nullopt,
                       std::optional<int> line = std::nullopt);

[[noreturn]] void extcde(int exitCode, std::ostream &os,
                         std::optional<std::string_view> msg = std::nullopt,
                         std::optional<std::string_view> file = std::nullopt,
                         std::optional<int> line = std::nullopt);

void warnng(std::ostream &os, std::string_view msg,
            std::optional<std::string_view> file = std::nullopt,
            std::optional<int> line = std::nullopt);

void writeUpdatingField(std::ostream &os, std::string_view fieldName);

void writeInterpolationInfo(std::ostream &os, const DateTime &time1,
                            const DateTime &time2);

} // namespace ww4_std_out
} // namespace ww4_utils
