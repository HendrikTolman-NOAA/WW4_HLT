/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file grid_rectilinear_init.h
 * @brief Header for the rectilinear grid initialization.
 * @details Defines the grid ingestion and domain decomposition routine for the
 *          rectilinear grid, shared by all rectilinear grid solvers.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-10-23
 * @date Last update : 2026-11-23
 */

#pragma once

#include <iostream>

namespace ww4_core {

// --- grid_rectilinear_init --------------------------------------------------
/**
 * @brief Initialization routine for the rectilinear grid (all solvers).
 * @details Reads NetCDF-UGRID grid geometry and metadata using
 * readRectilinearGrid, captures grid metadata, computes or reads domain
 * decomposition partition structures, extracts the local domain partition grid
 * with halo cells, and sets up inter-rank halo communication structures.
 * @param[in,out] os Output stream for logging and screen messages.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-10-22
 * @date Last update : 2026-11-23
 */
void grid_rectilinear_init(std::ostream &os = std::cout);

} // namespace ww4_core
