/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file solver_pr3.hpp
 * @brief Header for the PR3 solver.
 * @details Concrete implementation of the ISolver interface using PR3 dynamics.
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
 * @date Initial, 2026-06-24
 * @date Last update : 2026-06-24
 */

#pragma once

#include "ww4_core/solver.hpp"
#include <vector>

namespace ww4_core {

/**
 * @class SolverPr3
 * @brief Implementation of the PR3 solver.
 */
class SolverPr3 : public ISolver {
public:
  std::string_view getName() const noexcept override { return "PR3"; }

  void addSourceTerm(std::unique_ptr<ISourceTerm> source) override;

  void solve(std::span<double> data) override;

private:
  std::vector<std::unique_ptr<ISourceTerm>> sourceTerms_;
};

} // namespace ww4_core
