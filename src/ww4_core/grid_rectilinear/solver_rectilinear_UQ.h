/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file solver_rectilinear_UQ.h
 * @brief UQ solver for the rectilinear grid.
 * @details Concrete ISolver for the rectilinear grid using the Ultimate
 *          Quickest (UQ) propagation scheme. Source terms are handled by
 *          SolverBase.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-09-15
 * @date Last update : 2026-11-20
 */

#pragma once

#include "ww4_core/solver_base.h"
#include <span>
#include <string_view>

namespace ww4_core {

// --- SolverRectilinearUQ ----------------------------------------------------
/**
 * @class SolverRectilinearUQ
 * @brief UQ solver for the rectilinear grid.
 * @details Advances the model state on the rectilinear grid using the Ultimate
 *          Quickest (UQ) propagation scheme (mock propagation placeholder),
 *          then applies the shared source terms.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-09-15
 * @date Last update : 2026-11-20
 */
class SolverRectilinearUQ : public SolverBase {
public:
  /**
   * @brief Retrieves the solver scheme name.
   * @return String view representing the solver name ("UQ").
   */
  [[nodiscard]] std::string_view getName() const noexcept override {
    return "UQ";
  }

  /**
   * @brief Executes wave action propagation and evaluates source terms.
   * @param[in,out] data Span of wave action spectrum data values.
   */
  void solve(std::span<double> data) override;
};

} // namespace ww4_core
