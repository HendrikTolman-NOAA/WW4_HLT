/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file compute_all_sources.hpp
 * @brief Header for physical source term calculations in WW4 core.
 * @details Concrete implementation of ISourceTerm interface for computing all
 * source terms.
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

#pragma once

#include "ww4_core/source_term.hpp"
#include <span>

namespace ww4_core {

/**
 * @class ComputeAllSources
 * @brief Implementation for physical source term calculations.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
class ComputeAllSources : public ISourceTerm {
public:
  ComputeAllSources() = default;
  ~ComputeAllSources() override = default;

  [[nodiscard]] std::string_view getName() const noexcept override {
    return "ComputeAllSources";
  }

  void calculate(std::span<double> data) override;
};

} // namespace ww4_core
