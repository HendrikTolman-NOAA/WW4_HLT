/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_sourterm_BT4.cpp
 * @brief Implementation of BT4 bottom friction source terms.
 * @details Concrete subroutine for BT4 source terms in WW4 core.
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
 * @date Initial, 2026-09-24
 * @date Last update : 2026-09-24
 */

#include "ww4_core/ww4_source_terms/ww4_sourterm_BT4.h"
#include <algorithm>

namespace ww4_core {

void SourceTermBT4::calculate(std::span<double> data) {
  std::for_each(data.begin(), data.end(), [](double &val) {
    val -= 0.004; // Mock BT4 effect
  });
}

} // namespace ww4_core
