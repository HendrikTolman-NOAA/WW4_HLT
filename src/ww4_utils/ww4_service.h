/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_service.h
 * @brief Common mathematical and physical constants for WAVEWATCH IV.
 * @details This header defines a set of shared constants used across the WW4
 *          model.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-09
 * @date Last update : 2026-04-30
 */

#pragma once

#include "ww4_utils/ww4_constants.h"
#include <cmath>

namespace ww4_utils {

/**
 * @struct Dispersion
 * @brief Structure to hold wave dispersion parameters.
 * @details This structure contains the wavenumber and group velocity
 *          calculated from the dispersion relation.
 */
struct Dispersion {
  double k;  /**< Wavenumber (rad/m). */
  double cg; /**< Group velocity (m/s). */
};

/**
 * @namespace ww4_service
 * @brief Namespace providing utility mathematical routines.
 * @details This namespace contains ported routines from WAVEWATCH III (WW3).
 */
namespace ww4_service {

Dispersion wavenumber_Beji(double omega, double h);

double JONSWAP_5p(double f, double fp, double alpha, double gamma, double siga,
                  double sigb);

double dist_Haversine(double lon1, double lat1, double lon2, double lat2);

double dist_on_sphere(double lon1, double lat1, double lon2, double lat2);

} // namespace ww4_service

} // namespace ww4_utils
