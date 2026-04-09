/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_service.cpp
 * @brief Common mathematical and physical constants for WAVEWATCH IV.
 * @details This source file defines derived constants that cannot be evaluated
 *          using constexpr.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-09
 * @date Last Update, 2026-04-09
 *
 * @note This file is converted from WAVEWATCH III (WW3) source files
 *       constants.F90 and mod_constants.f90.
 *       Original author in WW3: Hendrik L. Tolman.
 */

#include "ww4_utils/ww4_service.hpp"
#include <cmath>

namespace ww4_utils {

namespace constants {

/** @brief Square root of gravity. */
const double SQRTG = std::sqrt(GRAV);

} // namespace constants

} // namespace ww4_utils
