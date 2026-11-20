/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file solver_base.h
 * @brief Common base class for all WAVEWATCH IV numerical solvers.
 * @details Implements the single source term approach shared by all solvers
 *          on all grids: registered ISourceTerm objects are evaluated in
 *          order, and ComputeAllSources is used if none were registered.
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
#include "ww4_core/source_term.h"
#include <memory>
#include <span>
#include <vector>

namespace ww4_core {

// --- SolverBase -------------------------------------------------------------
/**
 * @class SolverBase
 * @brief Base class providing the shared source term handling for solvers.
 * @details Concrete solvers derive from this class, implement getName() and
 *          solve(), and call applySourceTerms() from solve(). This keeps a
 *          single source term approach for all grid and solver combinations.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Kiro (Agentic AI)
 * @date Initial, 2026-11-20
 * @date Last update : 2026-11-20
 */
class SolverBase : public ISolver {
public:
  /**
   * @brief Registers an external physical source term component.
   * @param[in] source Unique pointer to an ISourceTerm implementation.
   */
  void addSourceTerm(std::unique_ptr<ISourceTerm> source) final;

protected:
  /**
   * @brief Evaluates all registered source terms on the data.
   * @details If no source terms were registered, ComputeAllSources is created
   *          through SchemeFactory and registered first.
   * @param[in,out] data Span of wave action spectrum data values.
   */
  void applySourceTerms(std::span<double> data);

private:
  /// Registered physical source terms evaluated during execution
  std::vector<std::unique_ptr<ISourceTerm>> sourceTerms_;
};

} // namespace ww4_core
