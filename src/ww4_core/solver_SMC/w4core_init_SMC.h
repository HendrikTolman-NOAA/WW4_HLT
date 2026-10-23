/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file w4core_init_SMC.h
 * @brief Header for the Spherical Multiple-Cell (SMC) grid solver
 * initialization.
 * @details Defines the initialization routine for the SMC grid solver.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-22
 * @date Last update : 2026-10-23
 */

#pragma once

#include <iostream>

namespace ww4_core {

// --- w4core_init_SMC --------------------------------------------------------
/**
 * @brief Initialization routine for the Spherical Multiple-Cell (SMC) grid
 * solver.
 * @param os Output stream for logging.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-22
 * @date Last update : 2026-10-22
 */
void w4core_init_SMC(std::ostream &os = std::cout);

} // namespace ww4_core
