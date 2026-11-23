/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file grid_triangular.cpp
 * @brief Implementation of the triangular unstructured grid type.
 * @details Grid initialization and solver factory for the triangular
 *          unstructured grid.
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

#include "ww4_core/grid_triangular/grid_triangular.h"
#include "ww4_core/grid_triangular/grid_triangular_init.h"
#include "ww4_core/grid_triangular/solver_explicit/solver_triangular_explicit.h"
#include "ww4_core/grid_triangular/solver_implicit/solver_triangular_implicit.h"
#include <stdexcept>
#include <string>

namespace ww4_core {

// --- init -------------------------------------------------------------------
void GridTriangular::init(std::ostream &os) { grid_triangular_init(os); }

// --- createSolver -----------------------------------------------------------
std::unique_ptr<ISolver>
GridTriangular::createSolver(std::string_view name) const {
  if (name == "implicit") {
    return std::make_unique<SolverTriangularImplicit>();
  }
  if (name == "explicit") {
    return std::make_unique<SolverTriangularExplicit>();
  }
  throw std::invalid_argument(
      "Unknown solver for triangular grid: " + std::string(name) +
      " (available: implicit, explicit)");
}

} // namespace ww4_core
