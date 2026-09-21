/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file source_terms_stub.hpp
 * @brief Header for the source terms stub in WW4 core.
 * @details Concrete stub implementation of ISourceTerm interface.
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
 * @date Last update : 2026-09-15
 */

#pragma once

#include "ww4_core/source_term.hpp"
#include <span>

namespace ww4_core {

/**
 * @class SourceTermsStub
 * @brief Stub implementation for source terms.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
class SourceTermsStub : public ISourceTerm {
public:
  SourceTermsStub() = default;
  ~SourceTermsStub() override = default;

  [[nodiscard]] std::string_view getName() const noexcept override {
    return "Stub";
  }

  void calculate(std::span<double> data) override;
};

} // namespace ww4_core
