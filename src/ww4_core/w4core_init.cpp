/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_init.cpp
 * @brief Implementation of the WAVEWATCH IV initialization routine.
 * @details This file implements the w4core_init routine.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last Update, 2026-04-06
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_init.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_run_config.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <iostream>

namespace ww4_core {

namespace {
ww4_utils::RunConfig globalRunConfig;
}

void w4core_init(const ww4_utils::DateTime & /*startTime*/) {
  // Load configuration
  globalRunConfig = ww4_utils::loadRunConfig("ww4_run_config.yml");

  if (globalRunConfig.produceStdOut) {
    ww4_utils::ww4_std_out::writeInitialOutput(std::cout, "Multi-grid shell");

    std::cout << "          Configuration settings :" << std::endl;

    std::string calType = "Standard";
    if (globalRunConfig.calendarType ==
        ww4_utils::TimeManagement::CalendarType::NoLeap) {
      calType = "NoLeap";
    } else if (globalRunConfig.calendarType ==
               ww4_utils::TimeManagement::CalendarType::ThreeSixtyDay) {
      calType = "ThreeSixtyDay";
    }

    std::cout << "            Calendar type      : " << calType << std::endl;
    std::cout << "            Screen output      : "
              << (globalRunConfig.produceStdOut ? "yes" : "no") << std::endl;
    std::cout << "            Log file           : "
              << (globalRunConfig.produceLogFile ? "yes" : "no") << std::endl;
    std::cout << std::endl;
  }
}

const ww4_utils::RunConfig &getRunConfig() { return globalRunConfig; }

} // namespace ww4_core
