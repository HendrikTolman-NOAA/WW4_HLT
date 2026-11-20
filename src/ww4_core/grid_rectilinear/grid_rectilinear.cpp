/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file grid_rectilinear.cpp
 * @brief Implementation of the rectilinear (regular) grid type.
 * @details Grid initialization and solver factory for the rectilinear (regular)
 *          grid.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Kiro (Agentic AI)
 * @date Initial, 2026-11-20
 * @date Last update : 2026-11-20
 */

#include "ww4_core/grid_rectilinear/grid_rectilinear.h"
#include "ww4_core/grid_rectilinear/grid_rectilinear_init.h"
#include "ww4_core/grid_rectilinear/solver_rectilinear_1UP.h"
#include "ww4_core/grid_rectilinear/solver_rectilinear_UQ.h"
#include <stdexcept>
#include <string>

namespace ww4_core {

// --- init -------------------------------------------------------------------
void GridRectilinear::init(std::ostream &os) { grid_rectilinear_init(os); }

// --- createSolver -----------------------------------------------------------
std::unique_ptr<ISolver>
GridRectilinear::createSolver(std::string_view name) const {
  if (name == "1UP") {
    return std::make_unique<SolverRectilinear1UP>();
  }
  if (name == "UQ") {
    return std::make_unique<SolverRectilinearUQ>();
  }
  throw std::invalid_argument("Unknown solver for rectilinear grid: " +
                              std::string(name) + " (available: 1UP, UQ)");
}

} // namespace ww4_core
