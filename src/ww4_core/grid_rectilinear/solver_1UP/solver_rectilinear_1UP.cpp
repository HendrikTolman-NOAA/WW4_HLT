/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file solver_rectilinear_1UP.cpp
 * @brief Implementation of the 1UP solver for the rectilinear grid.
 * @details Propagation step for the first-order upwind (1UP) propagation
 *          scheme; source terms via SolverBase.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Kiro (Agentic AI)
 * @date Initial, 2026-11-20
 * @date Last update : 2026-11-23
 */

#include "ww4_core/grid_rectilinear/solver_1UP/solver_rectilinear_1UP.h"
#include <algorithm>

namespace ww4_core {

// --- solve ------------------------------------------------------------------
void SolverRectilinear1UP::solve(std::span<double> data) {
  // === Perform numerical propagation =======================================
  std::for_each(data.begin(), data.end(), [](double &val) {
    val *= 1.01; // Mock propagation effect
  });

  // === Call integrated source terms ==========================================
  applySourceTerms(data);
}

} // namespace ww4_core
