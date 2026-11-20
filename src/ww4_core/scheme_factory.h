/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file scheme_factory.h
 * @brief Factory for creating grids, solvers and source terms at runtime.
 * @details Two-stage model selection: a grid type is selected first
 *          (createGrid), then one of the numerical solvers available on that
 *          grid (IGrid::createSolver, or the createSolver convenience
 *          function). Source terms are created independently of grid and
 *          solver (createSourceTerm).
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-06-24
 * @date Last update : 2026-11-20
 */

#pragma once

#include "ww4_core/grid.h"
#include "ww4_core/solver.h"
#include "ww4_core/source_term.h"
#include <memory>
#include <string>

namespace ww4_core {

// --- SchemeFactory ----------------------------------------------------------
/**
 * @class SchemeFactory
 * @brief Factory class for creating model grids, solvers and source terms.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-06-24
 * @date Last update : 2026-11-20
 */
class SchemeFactory {
public:
  /**
   * @brief Create a grid type (first selection stage).
   * @param name The name of the grid type ("rectilinear", "triangular",
   * "SMC").
   * @return A unique pointer to the created IGrid.
   * @throws std::invalid_argument If the grid type is unknown.
   */
  static std::unique_ptr<IGrid> createGrid(const std::string &name);

  /**
   * @brief Create a numerical solver for a grid type (second selection stage).
   * @details Convenience function for createGrid(gridName)->createSolver().
   * @param gridName The name of the grid type (see createGrid).
   * @param solverName The name of a solver available on that grid
   * ("1UP" or "UQ" for rectilinear, "implicit" or "explicit" for
   * triangular, "UKMET" for SMC).
   * @return A unique pointer to the created ISolver.
   * @throws std::invalid_argument If the grid type is unknown or the solver is
   * not available on that grid.
   */
  static std::unique_ptr<ISolver> createSolver(const std::string &gridName,
                                               const std::string &solverName);

  /**
   * @brief Create a source term scheme.
   * @param name The name of the scheme (e.g., "ST4").
   * @return A unique pointer to the created ISourceTerm.
   */
  static std::unique_ptr<ISourceTerm> createSourceTerm(const std::string &name);
};

} // namespace ww4_core
