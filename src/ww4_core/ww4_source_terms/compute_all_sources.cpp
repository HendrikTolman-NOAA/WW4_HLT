/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file compute_all_sources.cpp
 * @brief Implementation of physical source term calculations.
 * @details Concrete subroutine for physical source terms in WW4 core.
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
 * @date Last update : 2026-09-24
 */

#include "ww4_core/ww4_source_terms/compute_all_sources.h"
#include "ww4_core/scheme_factory.h"
#include "ww4_core/w4core_init.h"
#include "ww4_utils/ww4_run_config.h"

namespace ww4_core {

void ComputeAllSources::init() {
  const auto &config = getRunConfig();

  switch (config.inputDissipation) {
  case ww4_utils::InputDissipationScheme::ST1:
    inputDissipationTerm_ = SchemeFactory::createSourceTerm("ST1");
    break;
  case ww4_utils::InputDissipationScheme::ST2:
    inputDissipationTerm_ = SchemeFactory::createSourceTerm("ST2");
    break;
  case ww4_utils::InputDissipationScheme::ST4:
    inputDissipationTerm_ = SchemeFactory::createSourceTerm("ST4");
    break;
  case ww4_utils::InputDissipationScheme::ST6:
    inputDissipationTerm_ = SchemeFactory::createSourceTerm("ST6");
    break;
  default:
    inputDissipationTerm_ = nullptr;
    break;
  }

  switch (config.nonlinearInteractions) {
  case ww4_utils::NonlinearScheme::NL1:
    nonlinearTerm_ = SchemeFactory::createSourceTerm("NL1");
    break;
  case ww4_utils::NonlinearScheme::NL2:
    nonlinearTerm_ = SchemeFactory::createSourceTerm("NL2");
    break;
  case ww4_utils::NonlinearScheme::NL3:
    nonlinearTerm_ = SchemeFactory::createSourceTerm("NL3");
    break;
  default:
    nonlinearTerm_ = nullptr;
    break;
  }

  initialized_ = true;
}

void ComputeAllSources::calculate(std::span<double> data) {
  if (!getRunConfig().sourceTerms) {
    return;
  }

  if (!initialized_) {
    init();
  }

  if (inputDissipationTerm_) {
    inputDissipationTerm_->calculate(data);
  }

  if (nonlinearTerm_) {
    nonlinearTerm_->calculate(data);
  }
}

} // namespace ww4_core
