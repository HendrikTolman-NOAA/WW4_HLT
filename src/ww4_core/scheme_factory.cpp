/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file scheme_factory.cpp
 * @brief Implementation of the scheme factory.
 * @details Handles the instantiation of model grids, solvers and source terms.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), EJ Rainville, Kiro (Agentic AI)
 * @date Initial, 2026-06-24
 * @date Last update : 2026-11-20
 */

// Scheme Selection and Source-Term Computation (Factory Pattern setup)

#include "ww4_core/scheme_factory.h"
#include "ww4_core/source_terms/ComputeAllSources/ComputeAllSources.h"

// Grid definitions (each grid creates its own solvers)
#include "ww4_core/grid_SMC/grid_SMC.h"
#include "ww4_core/grid_rectilinear/grid_rectilinear.h"
#include "ww4_core/grid_triangular/grid_triangular.h"

// Source terms
// --- Linear Input
#include "ww4_core/source_terms/ww4_LN1/ww4_LN1.h"

// --- Input and Dissipation
#include "ww4_core/source_terms/ww4_ST1/ww4_ST1.h"
#include "ww4_core/source_terms/ww4_ST2/ww4_ST2.h"
#include "ww4_core/source_terms/ww4_ST4/ww4_ST4.h"
#include "ww4_core/source_terms/ww4_ST6/ww4_ST6.h"

// --- Nonlinear interactions
#include "ww4_core/source_terms/ww4_NL1/ww4_NL1.h"
#include "ww4_core/source_terms/ww4_NL2/ww4_NL2.h"
#include "ww4_core/source_terms/ww4_NL3/ww4_NL3.h"

// --- Bottom friction terms
#include "ww4_core/source_terms/ww4_BT1/ww4_BT1.h"
#include "ww4_core/source_terms/ww4_BT4/ww4_BT4.h"
#include <stdexcept>

namespace ww4_core {

// --- createGrid -------------------------------------------------------------
/**
 * @brief Instantiates a grid type by string name.
 * @param name Name of requested grid type.
 * @return Unique pointer to created grid.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Kiro (Agentic AI)
 * @date Initial, 2026-11-20
 * @date Last update : 2026-11-20
 */
std::unique_ptr<IGrid> SchemeFactory::createGrid(const std::string &name) {
  if (name == "rectilinear") {
    return std::make_unique<GridRectilinear>();
  }
  if (name == "triangular") {
    return std::make_unique<GridTriangular>();
  }
  if (name == "SMC") {
    return std::make_unique<GridSMC>();
  }
  throw std::invalid_argument("Unknown grid type: " + name);
}

// --- createSolver -----------------------------------------------------------
/**
 * @brief Instantiates a solver for a grid type by string names.
 * @param gridName Name of requested grid type.
 * @param solverName Name of requested solver on that grid.
 * @return Unique pointer to created solver.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-06-24
 * @date Last update : 2026-11-20
 */
std::unique_ptr<ISolver>
SchemeFactory::createSolver(const std::string &gridName,
                            const std::string &solverName) {
  return createGrid(gridName)->createSolver(solverName);
}

// --- createSourceTerm -------------------------------------------------------
/**
 * @brief Instantiates a source term scheme by string name.
 * @param name Name of requested source term.
 * @return Unique pointer to created source term.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-06-24
 * @date Last update : 2026-10-16
 */
std::unique_ptr<ISourceTerm>
SchemeFactory::createSourceTerm(const std::string &name) {
  if (name == "ComputeAllSources") {
    return std::make_unique<ComputeAllSources>();
  }
  if (name == "LN1") {
    return std::make_unique<SourceTermLN1>();
  }
  if (name == "ST1") {
    return std::make_unique<SourceTermST1>();
  }
  if (name == "ST2") {
    return std::make_unique<SourceTermST2>();
  }
  if (name == "ST4") {
    return std::make_unique<SourceTermST4>();
  }
  if (name == "ST6") {
    return std::make_unique<SourceTermST6>();
  }
  if (name == "NL1") {
    return std::make_unique<SourceTermNL1>();
  }
  if (name == "NL2") {
    return std::make_unique<SourceTermNL2>();
  }
  if (name == "NL3") {
    return std::make_unique<SourceTermNL3>();
  }
  if (name == "BT1") {
    return std::make_unique<SourceTermBT1>();
  }
  if (name == "BT4") {
    return std::make_unique<SourceTermBT4>();
  }
  throw std::invalid_argument("Unknown source term scheme: " + name);
}

} // namespace ww4_core
