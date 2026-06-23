/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file source_st4.cpp
 * @brief Implementation of the ST4 source term.
 * @details Concrete implementation of the ISourceTerm interface.
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

#include "ww4_core/source_st4.hpp"
#include <algorithm>

namespace ww4_core {

void SourceSt4::calculate(std::span<double> data) {
  // Placeholder implementation for ST4 source term logic.
  // In a real implementation, this would involve physics calculations.
  std::for_each(data.begin(), data.end(), [](double &val) {
    val += 0.5; // Mock source term effect
  });
}

} // namespace ww4_core
