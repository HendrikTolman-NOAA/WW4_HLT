/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_service.cpp
 * @brief Common mathematical and physical constants for WAVEWATCH IV
 *        and various service routines.
 * @details This source file holds subroutine codes by not the constants
 *          from constants.F90 as they are  implemented as constexpr in the header.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-10
 */

#include "ww4_utils/ww4_service.hpp"

namespace ww4_utils {

namespace constants {

// All constants are currently constexpr in the header.

} // namespace constants

double ww4_Service::JONSWAP_5p(double f, double fp, double alpha, double gamma,
                               double siga, double sigb) {
  if (f <= 0.0 || fp <= 0.0) {
    return 0.0;
  }

  // Physics factor approximating g^2 / (2 * PI)^4.
  // Maintained as 0.06175 for numerical consistency with WW3.
  const double FACTOR = 0.06175;

  double sigma = (f <= fp) ? siga : sigb;
  double f_ratio = fp / f;
  double r = std::exp(-0.5 * std::pow((f - fp) / (sigma * fp), 2));

  return FACTOR * alpha * std::pow(f, -5) *
         std::exp(-1.25 * std::pow(f_ratio, 4)) * std::pow(gamma, r);
}

} // namespace ww4_utils
