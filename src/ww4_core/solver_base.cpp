/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file solver_base.cpp
 * @brief Implementation of the common solver base class.
 * @details Shared source term registration and evaluation for all solvers.
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

#include "ww4_core/solver_base.h"
#include "ww4_core/scheme_factory.h"

namespace ww4_core {

// --- addSourceTerm ----------------------------------------------------------
void SolverBase::addSourceTerm(std::unique_ptr<ISourceTerm> source) {
  if (source) {
    sourceTerms_.push_back(std::move(source));
  }
}

// --- applySourceTerms -------------------------------------------------------
void SolverBase::applySourceTerms(std::span<double> data) {
  if (sourceTerms_.empty()) {
    addSourceTerm(SchemeFactory::createSourceTerm("ComputeAllSources"));
  }
  for (auto &source : sourceTerms_) {
    source->calculate(data);
  }
}

} // namespace ww4_core
