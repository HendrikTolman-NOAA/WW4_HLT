/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file solver_uq.cpp
 * @brief Implementation of the UQ solver.
 * @details Concrete implementation of the ISolver interface for regular grids.
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
 * @date Initial, 2026-09-15
 * @date Last update : 2026-09-24
 */

#include "ww4_core/solver_uq/solver_uq.h"
#include "ww4_core/w4core_init.h"
#include <algorithm>

namespace ww4_core {

void w4core_init_uq(std::ostream &os) {
  if (getRunConfig().produceStdOut &&
      getRunConfig().screenOutputLevel == ww4_utils::ScreenOutputLevel::Full) {
    os << "    Initializing UQ regular grid solver" << std::endl;
  }
}

void SolverRectangularGrid::init() { w4core_init_uq(); }

void SolverRectangularGrid::addSourceTerm(std::unique_ptr<ISourceTerm> source) {
  if (source) {
    sourceTerms_.push_back(std::move(source));
  }
}

void SolverRectangularGrid::solve(std::span<double> data) {
  // 1. Perform numerical propagation (dynamics for UQ scheme)
  std::for_each(data.begin(), data.end(), [](double &val) {
    val *= 1.01; // Mock propagation effect
  });

  // 2. Call integrated source terms (physics)
  for (auto &source : sourceTerms_) {
    source->calculate(data);
  }
}

} // namespace ww4_core
