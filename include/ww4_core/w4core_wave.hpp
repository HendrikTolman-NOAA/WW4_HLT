/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_wave.hpp
 * @brief Time stepping routine for the WAVEWATCH IV core.
 * @details This header defines the time stepping routine for the WW4 core.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last update, 2026-04-21
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#ifndef WW4_CORE_W4CORE_WAVE_HPP
#define WW4_CORE_W4CORE_WAVE_HPP

#include "ww4_utils/time_management.hpp"
#include <optional>

/**
 * @namespace ww4_core
 * @brief Core routines for WAVEWATCH IV.
 */
namespace ww4_core {

/**
 * @struct InputTimeData
 * @brief Structure to hold time tags for model inputs.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-21
 */
struct InputTimeData {
  std::optional<ww4_utils::DateTime> time1; ///< First time tag.
  std::optional<ww4_utils::DateTime> time2; ///< Second time tag.
  double maxStep = -1.0;                    ///< Maximum model time step.
};

/**
 * @struct WaveTimeData
 * @brief Structure to hold model time and time step information.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Last update, 2026-04-21
 */
struct WaveTimeData {
  double timeStep = -1.0;                       ///< Model time step.
  std::optional<ww4_utils::DateTime> modelTime; ///< Current model time.
  InputTimeData waterLevels;                    ///< Time data for water levels.
  InputTimeData currents;                       ///< Time data for currents.
  InputTimeData winds;                          ///< Time data for winds.
  InputTimeData iceConcentrations; ///< Time data for ice concentrations.
};

/**
 * @brief Time stepping routine for the WAVEWATCH IV core.
 * @details Performs a single time step or a sequence of time steps for the
 *          wave model core. Follows the architectural design of the
 *          time stepping in ww3_multi.F90 from WAVEWATCH III.
 * @param[in] startTime Simulation start time.
 * @param[in] endTime Simulation end time.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-03
 */
void w4core_wave(const ww4_utils::DateTime &startTime,
                 const ww4_utils::DateTime &endTime, std::ostream &os);

} // namespace ww4_core

#endif // WW4_CORE_W4CORE_WAVE_HPP
