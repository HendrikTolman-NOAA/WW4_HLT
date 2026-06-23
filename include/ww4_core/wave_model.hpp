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

#pragma once

#include "ww4_core/propagation_scheme.hpp"
#include "ww4_core/source_term.hpp"
#include <memory>
#include <vector>

namespace ww4_core {

class WaveModel {
public:
  void initialize(const std::string &propName, const std::string &sourceName);

  void setData(std::vector<double> initialData);

  void step();

  std::span<const double> getData() const { return data_; }

private:
  std::unique_ptr<IPropagationScheme> propagation_;
  std::unique_ptr<ISourceTerm> source_;
  std::vector<double> data_;
};

} // namespace ww4_core
