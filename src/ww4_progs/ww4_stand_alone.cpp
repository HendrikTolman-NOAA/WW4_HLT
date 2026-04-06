/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_stand_alone.cpp
 * @brief Stand-alone wave model program.
 * @details This program provides a simplified stand-alone environment for
 *          running the WAVEWATCH IV wave model core.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last Update, 2026-04-03
 * @note This program follows the structure of the stand-alone shell
 *       (ww3_shel.F90) in WAVEWATCH III.
 *       Original author of WW3 stand-alone shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_finl.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_core/w4core_wave.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_stand_alone_config.hpp"
#include <iostream>

/**
 * @brief Main entry point for the simplified ww4_stand_alone program.
 * @return 0 on success.
 */
int main() {
  std::cout << "Starting ww4_stand_alone ..." << std::endl;

  auto config = ww4_utils::loadStandAloneConfig("ww4_stand_alone.yml");
  if (!config) {
    std::cerr << "***************************************************"
              << std::endl;
    std::cerr << " ERROR: FAILED TO LOAD ww4_stand_alone.yml " << std::endl;
    std::cerr << "***************************************************"
              << std::endl;
    return 1;
  }

  std::cout << "  Start time: "
            << ww4_utils::TimeManagement::toFormattedString(config->startTime)
            << std::endl;
  std::cout << "  End time:   "
            << ww4_utils::TimeManagement::toFormattedString(config->endTime)
            << std::endl;

  ww4_core::w4core_init(config->startTime);
  ww4_core::w4core_wave(config->startTime, config->endTime);
  ww4_core::w4core_finl(config->endTime);

  std::cout << "Ending ww4_stand_alone" << std::endl;

  return 0;
}
