/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file source_st4.hpp
 * @brief Header for the ST4 source term scheme.
 * @details Concrete implementation of the ISourceTerm interface.
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

#include "ww4_core/source_term.hpp"

namespace ww4_core {

class SourceSt4 : public ISourceTerm {
public:
  std::string_view getName() const noexcept override { return "ST4"; }

  void calculate(std::span<double> data) override;
};

} // namespace ww4_core
