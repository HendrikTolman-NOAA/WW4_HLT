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
 * @date Last update, 2026-04-16
 * @note This program follows the structure of the stand-alone shell
 *       (ww3_shel.F90) in WAVEWATCH III.
 *       Original author of WW3 stand-alone shell: Hendrik L. Tolman.
 */

#include "ww4_core/w4core_finl.hpp"
#include "ww4_core/w4core_init.hpp"
#include "ww4_core/w4core_wave.hpp"
#include "ww4_utils/time_management.hpp"
#include "ww4_utils/ww4_stand_alone_config.hpp"
#include "ww4_utils/ww4_std_out.hpp"
#include <exception>
#include <filesystem>
#include <iostream>

/**
 * @brief Main entry point for the simplified ww4_stand_alone program.
 * @return 0 on success.
 */
int main(int argc, char **argv) {
  try {
    //
    // 0.  Program initialization -------------------------------------------
    // 0.0 Extract program name
    //
    std::string programName = "ww4_stand_alone";
    if (argc > 0) {
      programName = std::filesystem::path(argv[0]).stem().string();
    }

    //
    // 0.1 Load configuration from ww4_stand_alone.yml file
    //
    const auto config =
        ww4_utils::loadStandAloneConfig("ww4_stand_alone.yml", std::cout);
    if (!config) {
      ww4_utils::ww4_std_out::extcde(1, std::cerr,
                                     "Could not load stand-alone configuration",
                                     __FILE__, __LINE__);
    }

    ww4_utils::reportStandAloneConfig(*config, std::cout);

    //
    // 0.2 MPI initialization (if applicable) -------------------------------
    //
    //
    // 1.  Run initialization routine  --------------------------------------
    //
    ww4_core::w4core_init(config->startTime, programName, std::cout);

    //
    // 2.  Run time stepping routine  ----------------------------------------
    //
    ww4_core::w4core_wave(config->startTime, config->endTime, std::cout);

    //
    // 3.  Run finalization routine  -----------------------------------------
    //
    ww4_core::w4core_finl(config->endTime, std::cout);

    return 0;
  } catch (const std::exception &e) {
    ww4_utils::ww4_std_out::extcde(1, std::cerr, e.what(), __FILE__, __LINE__);
  } catch (...) {
    ww4_utils::ww4_std_out::extcde(1, std::cerr, "Unknown exception in main",
                                   __FILE__, __LINE__);
  }
}
