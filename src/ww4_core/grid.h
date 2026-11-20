/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file grid.h
 * @brief Interface for spatial grid types in WAVEWATCH IV.
 * @details Defines the abstract base class for the spatial grid types
 *          (rectilinear, triangular, SMC). A grid is selected first; it owns
 *          grid ingestion and domain decomposition, and acts as the factory
 *          for the numerical solvers that are available on that grid.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Kiro (Agentic AI)
 * @date Initial, 2026-11-20
 * @date Last update : 2026-11-20
 */

#pragma once

#include "ww4_core/solver.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <span>
#include <string_view>

namespace ww4_core {

// --- IGrid ------------------------------------------------------------------
/**
 * @class IGrid
 * @brief Abstract base class for WAVEWATCH IV spatial grid types.
 * @details First stage of the two-stage model selection: the grid type is
 *          selected first, and the grid then creates one of the numerical
 *          solvers available for that grid (second stage). All solvers share
 *          the same source term approach (see SolverBase).
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Kiro (Agentic AI)
 * @date Initial, 2026-11-20
 * @date Last update : 2026-11-20
 */
class IGrid {
public:
  virtual ~IGrid() = default;

  // --- Get the name of the grid type ----------------------------------------
  /**
   * @brief Get the name of the grid type.
   * @return String view containing the grid type name (e.g., "rectilinear").
   */
  [[nodiscard]] virtual std::string_view getName() const noexcept = 0;

  // --- Get the names of the solvers available on this grid ------------------
  /**
   * @brief Get the names of the numerical solvers available on this grid.
   * @return Span of solver names accepted by createSolver().
   */
  [[nodiscard]] virtual std::span<const std::string_view>
  getSolverNames() const noexcept = 0;

  // --- Check if a solver is available on this grid --------------------------
  /**
   * @brief Check if a numerical solver is available on this grid.
   * @param name Name of the solver.
   * @return True if createSolver(name) will succeed.
   */
  [[nodiscard]] bool hasSolver(std::string_view name) const noexcept {
    const auto names = getSolverNames();
    return std::ranges::find(names, name) != names.end();
  }

  // --- Initialize the grid --------------------------------------------------
  /**
   * @brief Grid ingestion, domain decomposition and halo setup.
   * @param[in,out] os Output stream for logging and screen messages.
   */
  virtual void init(std::ostream &os = std::cout) = 0;

  // --- Create a numerical solver for this grid ------------------------------
  /**
   * @brief Create one of the numerical solvers available on this grid.
   * @param name Name of the solver (one of getSolverNames()).
   * @return Unique pointer to the created ISolver.
   * @throws std::invalid_argument If the solver is not available on this grid.
   */
  [[nodiscard]] virtual std::unique_ptr<ISolver>
  createSolver(std::string_view name) const = 0;
};

} // namespace ww4_core
