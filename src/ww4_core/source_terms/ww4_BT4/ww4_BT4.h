/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_BT4.h
 * @brief Header for BT4 bottom friction source term calculations.
 * @details Concrete implementation of ISourceTerm for BT4 scheme.
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

// --- SourceTermBT4 ----------------------------------------------------------
/**
 * @class SourceTermBT4
 * @brief Implementation of BT4 bottom friction source terms.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial: 2026-09-24
 * @date Last update: 2026-09-29
 */
class SourceTermBT4 : public ISourceTerm {
public:
  SourceTermBT4() = default;
  ~SourceTermBT4() override = default;

  [[nodiscard]] std::string_view getName() const noexcept override {
    return "BT4";
  }

  void calculate(std::span<double> data) override;
};

} // namespace ww4_core
