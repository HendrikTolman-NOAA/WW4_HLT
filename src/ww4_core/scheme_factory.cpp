/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file scheme_factory.cpp
 * @brief Implementation of the scheme factory.
 * @details Handles the instantiation and assembly of model components.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 *
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 *
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-06-24
 * @date Last update : 2026-09-24
 */

#include "ww4_core/scheme_factory.h"
#include "ww4_core/solver_smc/solver_smc.h"
#include "ww4_core/solver_triangular/solver_triangular.h"
#include "ww4_core/solver_uq/solver_uq.h"
#include "ww4_core/ww4_source_terms/compute_all_sources.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_BT1.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_BT4.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_LN1.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_NL1.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_NL2.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_NL3.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_ST1.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_ST2.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_ST4.h"
#include "ww4_core/ww4_source_terms/ww4_sourterm_ST6.h"
#include <stdexcept>

namespace ww4_core {

std::unique_ptr<ISolver> SchemeFactory::createSolver(const std::string &name) {
  if (name == "UQ" || name == "regular_uq" || name == "uq" ||
      name == "RectangularGrid" || name == "rectangular_grid") {
    return std::make_unique<SolverRectangularGrid>();
  }
  if (name == "Triangular" || name == "triangular" ||
      name == "tbd_triangular" || name == "TriangularGrid" ||
      name == "triangular_grid") {
    return std::make_unique<SolverTriangularGrid>();
  }
  if (name == "SMC" || name == "smc" || name == "SMCGrid" ||
      name == "smc_grid") {
    return std::make_unique<SolverSMCGrid>();
  }
  throw std::invalid_argument("Unknown solver: " + name);
}

std::unique_ptr<ISourceTerm>
SchemeFactory::createSourceTerm(const std::string &name) {
  if (name == "ComputeAllSources" || name == "compute_all_sources" ||
      name == "Stub" || name == "stub") {
    return std::make_unique<ComputeAllSources>();
  }
  if (name == "ST1" || name == "st1") {
    return std::make_unique<SourceTermST1>();
  }
  if (name == "ST2" || name == "st2") {
    return std::make_unique<SourceTermST2>();
  }
  if (name == "ST4" || name == "st4") {
    return std::make_unique<SourceTermST4>();
  }
  if (name == "ST6" || name == "st6") {
    return std::make_unique<SourceTermST6>();
  }
  if (name == "NL1" || name == "nl1") {
    return std::make_unique<SourceTermNL1>();
  }
  if (name == "NL2" || name == "nl2") {
    return std::make_unique<SourceTermNL2>();
  }
  if (name == "NL3" || name == "nl3") {
    return std::make_unique<SourceTermNL3>();
  }
  if (name == "LN1" || name == "ln1") {
    return std::make_unique<SourceTermLN1>();
  }
  if (name == "BT1" || name == "bt1") {
    return std::make_unique<SourceTermBT1>();
  }
  if (name == "BT4" || name == "bt4") {
    return std::make_unique<SourceTermBT4>();
  }
  throw std::invalid_argument("Unknown source term scheme: " + name);
}

} // namespace ww4_core
