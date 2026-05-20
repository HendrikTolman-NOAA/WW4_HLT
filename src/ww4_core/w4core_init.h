/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_init.h
 * @brief Initialization routine for the WAVEWATCH IV core.
 * @details This header defines the initialization routine for the WW4 core.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Last update : 2026-05-20
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#pragma once

#include "ww4_core/w4core_wave.h"
#include "ww4_utils/time_management.h"
#include "ww4_utils/ww4_input_utils.h"
#include "ww4_utils/ww4_run_config.h"
#include <fstream>
#include <iostream>

namespace ww4_core {

void w4core_init(const ww4_utils::DateTime &startTime,
                 std::string_view programName, std::ostream &os);

void resetInternalState() noexcept;

const std::string &getProgramName();

const ww4_utils::RunConfig &getRunConfig();

ww4_utils::RunConfig &getMutableRunConfig();

std::ofstream &getLogFileStream();

const ww4_utils::waveTimeData &getWaveTimeData();

void updateWaveModelTime(const ww4_utils::DateTime &time);

void updateWaveInputTime(ww4_utils::InputType type,
                         const ww4_utils::intTimeData &data);

} // namespace ww4_core
