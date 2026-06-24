/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file wave_model.hpp
 * @brief Orchestration for the WW4 wave model.
 * @details Manages simulation state and executes model phases.
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

#ifndef WW4_CORE_WAVE_MODEL_HPP
#define WW4_CORE_WAVE_MODEL_HPP

#include "ww4_core/propagation_scheme.hpp"
#include <memory>
#include <vector>

namespace ww4_core {

/**
 * @class WaveModel
 * @brief Main engine for the wave model simulation.
 */
class WaveModel {
public:
  /**
   * @brief Initialize the model with an assembled propagation scheme.
   * @param propagation Unique pointer to the assembled scheme (including
   * physics).
   */
  void initialize(std::unique_ptr<IPropagationScheme> propagation);

  /**
   * @brief Set the simulation data.
   * @param initialData Vector of initial wave data.
   */
  void setData(std::vector<double> initialData);

  /**
   * @brief Execute a single simulation step.
   */
  void step();

  /**
   * @brief Get a view of the current data.
   * @return Constant span of the model data.
   */
  std::span<const double> getData() const { return data_; }

private:
  std::unique_ptr<IPropagationScheme> propagation_;
  std::vector<double> data_;
};

} // namespace ww4_core

#endif // WW4_CORE_WAVE_MODEL_HPP
