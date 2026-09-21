/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file solver_smc.hpp
 * @brief Header for the SMC grid solver stub.
 * @details Concrete implementation of the ISolver interface using the UK
 * MetOffice SMC grid scheme.
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

#include "ww4_core/solver.hpp"
#include "ww4_core/source_term.hpp"
#include <memory>
#include <span>
#include <vector>

namespace ww4_core {

/**
 * @class SolverSMC
 * @brief Implementation of the SMC grid solver stub.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
class SolverSMC : public ISolver {
public:
  SolverSMC() = default;
  ~SolverSMC() override = default;

  [[nodiscard]] std::string_view getName() const noexcept override {
    return "SMC";
  }

  void addSourceTerm(std::unique_ptr<ISourceTerm> source) override;

  void solve(std::span<double> data) override;

private:
  std::vector<std::unique_ptr<ISourceTerm>> sourceTerms_;
};

} // namespace ww4_core
