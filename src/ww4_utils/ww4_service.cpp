/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_service.cpp
 * @brief Implementation of service routines.
 * @details This file implements the utility routines for ww4Service,
 *          providing core algorithms ported from WAVEWATCH III.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-09
 * @date Last Update, 2026-04-10
 *
 * @note This file is converted from WAVEWATCH III (WW3) source file
 *       w3servmd.F90.
 */

#include "ww4_utils/ww4_service.hpp"

#include <cmath>

namespace ww4_utils {

double ww4Service::JONSWAP_5p(double f, double alfa, double fp, double yln,
                              double siga, double sigb) noexcept {
  /**
   * @details Conversion of EJ5P from WW3 w3servmd.F90.
   *          Initial author: G. Ph. van Vledder.
   */

  constexpr double eps = 1.0E-4;
  constexpr double exp_min = -180.0;

  if (f < eps) {
    return 0.0;
  }

  // GRAV**2 / (2 * PI)**4 = 9.806**2 / (2 * 3.141592653589793)**4 approx
  // 0.0617505... Original code used 0.06175.
  const double a = alfa * 0.06175 / std::pow(f, 5);
  double b = -1.25 * std::pow(fp / f, 4);

  if (b < exp_min) {
    b = exp_min;
  }

  if (yln < eps) {
    return a * std::exp(b);
  }

  const double sig = (f <= fp) ? siga : sigb;
  double c = -0.5 * std::pow((f - fp) / (sig * fp), 2);

  if (c < exp_min) {
    c = exp_min;
  }

  return a * std::exp(b + std::exp(c) * yln);
}

} // namespace ww4_utils
