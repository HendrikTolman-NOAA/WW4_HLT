/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file propagation_scheme.hpp
 * @brief Interface for numerical propagation schemes in WAVEWATCH IV.
 * @details Defines the abstract base class for different propagation
 * algorithms.
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

#include <span>
#include <string_view>

namespace ww4_core {

class IPropagationScheme {
public:
  virtual ~IPropagationScheme() = default;

  virtual std::string_view getName() const noexcept = 0;

  virtual void propagate(std::span<double> data) = 0;
};

} // namespace ww4_core
