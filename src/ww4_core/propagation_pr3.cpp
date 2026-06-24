/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file propagation_pr3.cpp
 * @brief Implementation of the PR3 propagation scheme.
 * @details Concrete implementation of the IPropagationScheme interface.
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
 * @date Initial, 2026-06-08
 * @date Last update : 2026-06-08
 */

#include "ww4_core/propagation_pr3.hpp"
#include <algorithm>

namespace ww4_core {

void PropagationPr3::addSourceTerm(std::unique_ptr<ISourceTerm> source) {
  if (source) {
    sourceTerms_.push_back(std::move(source));
  }
}

void PropagationPr3::propagate(std::span<double> data) {
  // 1. Perform numerical propagation
  std::for_each(data.begin(), data.end(), [](double &val) {
    val *= 1.01; // Mock propagation effect
  });

  // 2. Call integrated source terms
  for (auto &source : sourceTerms_) {
    source->calculate(data);
  }
}

} // namespace ww4_core
