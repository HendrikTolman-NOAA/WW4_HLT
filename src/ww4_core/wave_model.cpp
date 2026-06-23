/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file wave_model.cpp
 * @brief Implementation of the WaveModel class.
 * @details Orchestrates the model initialization and execution loop.
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

#include "ww4_core/wave_model.hpp"
#include "ww4_core/scheme_factory.hpp"

namespace ww4_core {

void WaveModel::initialize(const std::string &propName,
                           const std::string &sourceName) {
  propagation_ = SchemeFactory::createPropagationScheme(propName);
  source_ = SchemeFactory::createSourceTerm(sourceName);
}

void WaveModel::setData(std::vector<double> initialData) {
  data_ = std::move(initialData);
}

void WaveModel::step() {
  if (propagation_) {
    propagation_->propagate(data_);
  }
  if (source_) {
    source_->calculate(data_);
  }
}

} // namespace ww4_core
