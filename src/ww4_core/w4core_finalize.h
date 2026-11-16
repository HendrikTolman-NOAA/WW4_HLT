/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_finalize.h
 * @brief Finalization routine for the WAVEWATCH IV core.
 * @details This header defines the finalization routine for the WW4 core.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * The National Weather Service (NWS) often uses Generative SI (GenSI, formally
 * GenAI) tools for code development and refactoring. Whenever such tools are
 * used, NWS requires disclosure and a full human review of changes before they
 * are applied to NWS repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI/SI)
 * @date Initial: 2026-04-03
 * @date Last update: 2026-11-16
 * @note The architectural design of this routine follows the structure of
 *       the multi-grid shell (ww3_multi.F90) in WAVEWATCH III.
 *       Original author of WW3 multi-grid shell: Hendrik L. Tolman.
 */

#pragma once

#include "ww4_utils/time_management.h"

namespace ww4_core {

// --- w4core_finalize --------------------------------------------------------
/**
 * @brief Finalization routine for the WAVEWATCH IV core.
 * @details Performs all necessary cleanup and final reporting for the wave
 * model core.
 * @param endTime Simulation end time.
 * @param os Output stream for reporting.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial: 2026-04-03
 * @date Last update: 2026-09-25
 */
void w4core_finalize(const ww4_utils::DateTime &endTime, std::ostream &os);

} // namespace ww4_core
