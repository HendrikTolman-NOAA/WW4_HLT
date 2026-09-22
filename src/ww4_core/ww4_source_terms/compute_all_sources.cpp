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
 * @date Last update : 2026-09-15
 */

#include "ww4_core/ww4_source_terms/compute_all_sources.hpp"
#include <algorithm>

namespace ww4_core {

void ComputeAllSources::calculate(std::span<double> data) {
  // Subroutine for source term calculations.
  std::for_each(data.begin(), data.end(), [](double &val) {
    val += 0.1; // Mock source term effect
  });
}

} // namespace ww4_core
