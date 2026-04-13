/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_service.cpp
 * @brief Common mathematical and physical constants for WAVEWATCH IV
 *        and various service routines.
 * @details This source file holds subroutine codes but not the constants
 *          from constants.F90 as they are  implemented as constexpr in the
 *          header.
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

namespace ww4_service {

double JONSWAP_5p(double f, double fp, double alpha, double gamma, double siga,
                  double sigb) {
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

double dist_Haversine(double lon1, double lat1, double lon2, double lat2) {
  // Compute differences in latitude and longitude in radians
  double dlat = (lat2 - lat1) * constants::DERA;
  double dlon = (lon2 - lon1) * constants::DERA;

  // Compute the haversine of the central angle
  double a =
      std::pow(std::sin(dlat / 2.0), 2) + std::cos(lat1 * constants::DERA) *
                                              std::cos(lat2 * constants::DERA) *
                                              std::pow(std::sin(dlon / 2.0), 2);

  // Compute the angular distance (c), ensuring no precision issues
  double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(std::max(0.0, 1.0 - a)));

  // Compute the spherical distance in radians
  return c;
}

double dist_on_sphere(double lon1, double lat1, double lon2, double lat2) {
  // Compute the haversine distance in radians
  double dist_rad = dist_Haversine(lon1, lat1, lon2, lat2);

  // Convert to meters using the radius of the earth
  return dist_rad * constants::RADIUS;
}

Dispersion wavenumber_Beji(double omega, double h) {
  // Logic ported from WW3 routine WAVNU3 in w3dispmd.ftn
  // Original authors: Aron Roland based on Beji (2013)

  // Avoid division by zero and handle deep water/undefined depth cases
  if (omega <= 0.0 || h <= 0.0) {
    return {0.0, 0.0};
  }

  const double kh0 = (omega * omega * h) / constants::GRAV;
  const double kdmax = 20.0;

  // Beji's improved Eckart approximation
  const double tmp = 1.55 + 1.3 * kh0 + 0.216 * kh0 * kh0;
  const double kh =
      kh0 *
      (1.0 + std::pow(kh0, 1.09) * (1.0 / std::exp(std::min(kdmax, tmp)))) /
      std::sqrt(std::tanh(std::min(kdmax, kh0)));

  double k = kh / h;

  // Calculate group velocity
  double cg = 0.5 * (1.0 + (2.0 * kh / std::sinh(std::min(kdmax, 2.0 * kh)))) *
              omega / k;

  return {k, cg};
}

} // namespace ww4_service

} // namespace ww4_utils
