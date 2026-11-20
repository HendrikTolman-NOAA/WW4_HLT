/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file grid_SMC.cpp
 * @brief Implementation of the Spherical Multiple-Cell (SMC) grid type.
 * @details Grid initialization and solver factory for the Spherical
 *          Multiple-Cell (SMC) grid.
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

#include "ww4_core/grid_SMC/grid_SMC.h"
#include "ww4_core/grid_SMC/grid_SMC_init.h"
#include "ww4_core/grid_SMC/solver_SMC_UKMET.h"
#include <stdexcept>
#include <string>

namespace ww4_core {

// --- init -------------------------------------------------------------------
void GridSMC::init(std::ostream &os) { grid_SMC_init(os); }

// --- createSolver -----------------------------------------------------------
std::unique_ptr<ISolver> GridSMC::createSolver(std::string_view name) const {
  if (name == "UKMET") {
    return std::make_unique<SolverSMCUKMET>();
  }
  throw std::invalid_argument("Unknown solver for SMC grid: " +
                              std::string(name) + " (available: UKMET)");
}

} // namespace ww4_core
