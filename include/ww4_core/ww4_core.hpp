/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_core.hpp
 * @brief Core routines for the WAVEWATCH IV wave model.
 * @details This header defines the initialization, time stepping, and
 *          finalization routines for the WW4 core.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-03
 * @date Last Update, 2026-04-03
 * @note The architectural design of these routines follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#ifndef WW4_CORE_WW4_CORE_HPP
#define WW4_CORE_WW4_CORE_HPP

/**
 * @namespace ww4_core
 * @brief Core routines for WAVEWATCH IV.
 */
namespace ww4_core {

/**
 * @brief Initialization routine for the WAVEWATCH IV core.
 * @details Performs all necessary setup for the wave model core.
 *          Follows the architectural design of the initialization in
 *          ww3_multi.F90 from WAVEWATCH III.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-03
 */
void w4core_init();

/**
 * @brief Time stepping routine for the WAVEWATCH IV core.
 * @details Performs a single time step or a sequence of time steps for the
 *          wave model core. Follows the architectural design of the
 *          time stepping in ww3_multi.F90 from WAVEWATCH III.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-03
 */
void w4core_wave();

/**
 * @brief Finalization routine for the WAVEWATCH IV core.
 * @details Performs all necessary cleanup and final reporting for the
 *          wave model core. Follows the architectural design of the
 *          finalization in ww3_multi.F90 from WAVEWATCH III.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date 2026-04-03
 */
void w4core_finl();

} // namespace ww4_core

#endif // WW4_CORE_WW4_CORE_HPP
