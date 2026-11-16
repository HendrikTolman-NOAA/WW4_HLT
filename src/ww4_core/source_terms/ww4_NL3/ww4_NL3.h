/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_NL3.h
 * @brief Header for NL3 nonlinear interaction source term calculations.
 * @details Concrete implementation of ISourceTerm for NL3 scheme.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * The National Weather Service (NWS) often uses Generative SI (GenSI, formally
 * GenAI) tools for code development and refactoring. Whenever such tools are
 * used, NWS requires disclosure and a full human review of changes before they
 * are applied to NWS repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI/SI)
 * @date Initial: 2026-09-24
 * @date Last update: 2026-11-16
 */

#pragma once

#include "ww4_core/source_term.h"
#include <span>

namespace ww4_core {

// --- SourceTermNL3 ----------------------------------------------------------
/**
 * @class SourceTermNL3
 * @brief Implementation of NL3 nonlinear interaction source terms.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial: 2026-09-24
 * @date Last update: 2026-09-29
 */
class SourceTermNL3 : public ISourceTerm {
public:
  SourceTermNL3() = default;
  ~SourceTermNL3() override = default;

  [[nodiscard]] std::string_view getName() const noexcept override {
    return "NL3";
  }

  void calculate(std::span<double> data) override;
};

} // namespace ww4_core
