/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file propagation_pr3.hpp
 * @brief Header for the PR3 propagation scheme.
 * @details Concrete implementation of the IPropagationScheme interface.
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

namespace ww4_core {

class PropagationPr3 : public IPropagationScheme {
public:
  std::string_view getName() const noexcept override { return "PR3"; }

  void propagate(std::span<double> data) override;
};

} // namespace ww4_core
