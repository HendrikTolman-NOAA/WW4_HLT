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
 * @date Last Update, 2026-04-07
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_wave.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

namespace ww4_core {

void w4core_wave(const ww4_utils::DateTime &startTime,
                 const ww4_utils::DateTime &endTime) {
  try {
    //
    // Report starting time stepping
    if (getRunConfig().produceStdOut) {
      std::cout << "          * Time stepping (w4core_wave) from: "
                << ww4_utils::TimeManagement::toFormattedString(startTime)
                << " to: "
                << ww4_utils::TimeManagement::toFormattedString(endTime)
                << std::endl;
      //
      ww4_utils::reportRunConfig(getRunConfig(), std::cout);
    }

    //
    // Sleep for 2 seconds to facilitate testing of run-time output
    //
    std::this_thread::sleep_for(std::chrono::seconds(2));
  } catch (const std::exception &e) {
    ww4_utils::ww4_std_out::extcde(1, std::cerr, e.what(), __FILE__, __LINE__);
  } catch (...) {
    ww4_utils::ww4_std_out::extcde(
        1, std::cerr, "Unknown exception in w4core_wave", __FILE__, __LINE__);
  }
}

} // namespace ww4_core
