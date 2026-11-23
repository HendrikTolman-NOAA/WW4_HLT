/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file solver_triangular_implicit.cpp
 * @brief Implementation of the implicit solver for the triangular unstructured
 * grid.
 * @details Propagation step for the implicit propagation scheme; source terms
 *          via SolverBase.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-09-15
 * @date Last update : 2026-11-23
 */

#include "ww4_core/grid_triangular/solver_implicit/solver_triangular_implicit.h"
#include <algorithm>

namespace ww4_core {

// --- solve ------------------------------------------------------------------
void SolverTriangularImplicit::solve(std::span<double> data) {
  // === Perform numerical propagation =======================================
  std::for_each(data.begin(), data.end(), [](double &val) {
    val *= 1.02; // Mock propagation effect
  });

  // === Call integrated source terms ==========================================
  applySourceTerms(data);
}

} // namespace ww4_core
