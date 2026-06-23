/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file scheme_factory.hpp
 * @brief Factory for creating propagation and source term schemes at runtime.
 * @details Implements a registry-based factory for scheme instantiation.
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

#pragma once

#include "ww4_core/propagation_scheme.hpp"
#include "ww4_core/source_term.hpp"
#include <memory>
#include <string>

namespace ww4_core {

class SchemeFactory {
public:
  static std::unique_ptr<IPropagationScheme>
  createPropagationScheme(const std::string &name);

  static std::unique_ptr<ISourceTerm> createSourceTerm(const std::string &name);
};

} // namespace ww4_core
