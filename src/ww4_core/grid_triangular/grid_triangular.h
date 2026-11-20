/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file grid_triangular.h
 * @brief The triangular unstructured grid type.
 * @details Grid ingestion for the triangular unstructured grid and factory for
 *          the solvers available on it (implicit, explicit).
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

#include "ww4_core/grid.h"
#include <array>
#include <iostream>
#include <memory>
#include <span>
#include <string_view>

namespace ww4_core {

// --- GridTriangular ---------------------------------------------------------
/**
 * @class GridTriangular
 * @brief The triangular unstructured grid and its solver factory.
 * @details Owns grid ingestion and domain decomposition for the triangular
 *          unstructured grid and creates the solvers available on it: implicit,
 *          explicit.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Kiro (Agentic AI)
 * @date Initial, 2026-11-20
 * @date Last update : 2026-11-20
 */
class GridTriangular : public IGrid {
public:
  /// Names of the numerical solvers available on this grid
  static constexpr std::array<std::string_view, 2> solverNames{"implicit",
                                                               "explicit"};

  /**
   * @brief Retrieves the grid type name.
   * @return String view representing the grid type name ("triangular").
   */
  [[nodiscard]] std::string_view getName() const noexcept override {
    return "triangular";
  }

  /**
   * @brief Names of the numerical solvers available on this grid.
   * @return Span over solverNames (implicit, explicit).
   */
  [[nodiscard]] std::span<const std::string_view>
  getSolverNames() const noexcept override {
    return solverNames;
  }

  /**
   * @brief Grid ingestion and domain decomposition using grid_triangular_init.
   * @param[in,out] os Output stream for logging and screen messages.
   */
  void init(std::ostream &os = std::cout) override;

  /**
   * @brief Create a numerical solver for this grid.
   * @param name Solver name (implicit, explicit).
   * @return Unique pointer to the created ISolver.
   * @throws std::invalid_argument If the solver is not available on this grid.
   */
  [[nodiscard]] std::unique_ptr<ISolver>
  createSolver(std::string_view name) const override;
};

} // namespace ww4_core
