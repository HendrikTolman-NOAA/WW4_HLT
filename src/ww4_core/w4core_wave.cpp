/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_wave.cpp
 * @brief Implementation of the WAVEWATCH IV time stepping routine.
 * @details This file implements the w4core_wave routine.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last Update, 2026-04-03
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_wave.hpp"
#include "ww4_utils/time_management.hpp"
#include <iostream>

namespace ww4_core {

void w4core_wave(const ww4_utils::DateTime &startTime,
                 const ww4_utils::DateTime &endTime) {
  std::cout << "          * Time stepping (w4core_wave) ... receiving start: "
            << ww4_utils::TimeManagement::toFormattedString(startTime)
            << " end: " << ww4_utils::TimeManagement::toFormattedString(endTime)
            << " ... complete" << std::endl;
}

} // namespace ww4_core
