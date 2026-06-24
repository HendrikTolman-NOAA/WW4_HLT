/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file source_term.hpp
 * @brief Interface for source term calculations in WAVEWATCH IV.
 * @details Defines the abstract base class for different physical source terms.
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
 * @date Initial, 2026-06-24
 * @date Last update : 2026-06-24
 */

#pragma once

#include <span>
#include <string_view>

namespace ww4_core {

class ISourceTerm {
public:
  virtual ~ISourceTerm() = default;

  virtual std::string_view getName() const noexcept = 0;

  virtual void calculate(std::span<double> data) = 0;
};

} // namespace ww4_core
