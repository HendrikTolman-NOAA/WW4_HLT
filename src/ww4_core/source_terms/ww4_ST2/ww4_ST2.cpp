/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_ST2.cpp
 * @brief Implementation of ST2 input and dissipation source term calculations.
 * @details Concrete function for ST2 input and dissipation source terms.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * The National Weather Service (NWS) often uses Generative SI (GenSI, formally
 * GenAI) tools for code development and refactoring. Whenever such tools are
 * used, NWS requires disclosure and a full human review of changes before they
 * are applied to NWS repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI/SI)
 * @date Initial: 2026-09-24
 * @date Last update: 2026-11-16
 */

#include "ww4_core/source_terms/ww4_ST2/ww4_ST2.h"
#include <algorithm>

namespace ww4_core {

// --- calculate --------------------------------------------------------------
void SourceTermST2::calculate(std::span<double> data) {
  std::for_each(data.begin(), data.end(), [](double &val) { val += 0.02; });
}

} // namespace ww4_core
