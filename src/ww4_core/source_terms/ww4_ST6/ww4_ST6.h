/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_ST6.h
 * @brief Header for ST6 input and dissipation source term calculations.
 * @details Concrete implementation of ISourceTerm for ST6 scheme.
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

// --- SourceTermST6 ----------------------------------------------------------
/**
 * @class SourceTermST6
 * @brief Implementation of ST6 input and dissipation source terms.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial: 2026-09-24
 * @date Last update: 2026-09-29
 */
class SourceTermST6 : public ISourceTerm {
public:
  SourceTermST6() = default;
  ~SourceTermST6() override = default;

  [[nodiscard]] std::string_view getName() const noexcept override {
    return "ST6";
  }

  void calculate(std::span<double> data) override;
};

} // namespace ww4_core
