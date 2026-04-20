/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_finl.hpp
 * @brief Finalization routine for the WAVEWATCH IV core.
 * @details This header defines the finalization routine for the WW4 core.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last update, 2026-04-20
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#ifndef WW4_CORE_W4CORE_FINL_HPP
#define WW4_CORE_W4CORE_FINL_HPP

#include "ww4_utils/time_management.hpp"

/**
 * @namespace ww4_core
 * @brief Core routines for WAVEWATCH IV.
 */
namespace ww4_core {

/**
 * @brief Finalization routine for the WAVEWATCH IV core.
 * @details Performs all necessary cleanup and final reporting for the
 *          wave model core. Follows the architectural design of the
 *          finalization in ww3_multi.F90 from WAVEWATCH III.
 * @param[in] endTime Simulation end time.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-03
 */
void w4core_finl(const ww4_utils::DateTime &endTime, std::ostream &os);

} // namespace ww4_core

#endif // WW4_CORE_W4CORE_FINL_HPP
