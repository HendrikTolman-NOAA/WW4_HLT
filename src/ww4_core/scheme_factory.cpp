/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file scheme_factory.cpp
 * @brief Implementation of the scheme factory.
 * @details Handles the instantiation of concrete schemes based on identifiers.
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

#include "ww4_core/scheme_factory.hpp"
#include "ww4_core/propagation_pr3.hpp"
#include "ww4_core/source_st4.hpp"
#include <stdexcept>

namespace ww4_core {

std::unique_ptr<IPropagationScheme>
SchemeFactory::createPropagationScheme(const std::string &name) {
  if (name == "PR3") {
    return std::make_unique<PropagationPr3>();
  }
  throw std::invalid_argument("Unknown propagation scheme: " + name);
}

std::unique_ptr<ISourceTerm>
SchemeFactory::createSourceTerm(const std::string &name) {
  if (name == "ST4") {
    return std::make_unique<SourceSt4>();
  }
  throw std::invalid_argument("Unknown source term scheme: " + name);
}

} // namespace ww4_core
