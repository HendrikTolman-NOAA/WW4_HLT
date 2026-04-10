/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_service.hpp
 * @brief Common mathematical and physical constants for WAVEWATCH IV.
 * @details This header defines a set of shared constants used across the WW4
 *          model.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-09
 * @date Last update, 2026-04-10
 */

#ifndef WW4_UTILS_WW4_SERVICE_HPP
#define WW4_UTILS_WW4_SERVICE_HPP

#include <cmath>

namespace ww4_utils {

/**
 * @namespace constants
 * @brief Namespace containing all physical and mathematical constants.
 * @details This namespace contains constants converted from the WAVEWATCH III
 *          (WW3) source file constants.F90.
 *          Original author in WW3: Hendrik L. Tolman.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-09
 * @date Last update, 2026-04-10
 */
namespace constants {

// Mathematical constants
/** @brief Value of Pi. */
constexpr double PI = 3.14159265358979323846;
/** @brief 2 * Pi. */
constexpr double TPI = 2.0 * PI;
/** @brief 0.5 * Pi. */
constexpr double HPI = 0.5 * PI;
/** @brief Inverse of 2 * Pi. */
constexpr double TPIINV = 1.0 / TPI;
/** @brief Inverse of 0.5 * Pi. */
constexpr double HPIINV = 1.0 / HPI;
/** @brief Conversion factor from radians to degrees. */
constexpr double RADE = 180.0 / PI;
/** @brief Conversion factor from degrees to radians. */
constexpr double DERA = PI / 180.0;

// Physical constants
/** @brief Acceleration of gravity (m/s^2). (constants.F90) */
constexpr double GRAV = 9.806;
/** @brief Density of water (kg/m^3). (constants.F90) */
constexpr double DWAT = 1000.0;
/** @brief Density of air (kg/m^3). (constants.F90) */
constexpr double DAIR = 1.225;
/** @brief Kinematic viscosity of air (m^2/s). */
constexpr double NU_AIR = 1.4e-5;
/** @brief Kinematic viscosity of water (m^2/s). (constants.F90) */
constexpr double NU_WATER = 1.31e-6;
/** @brief Specific gravity of sediments (N.D.). */
constexpr double SED_SG = 2.65;
/** @brief von Karman's constant (N.D.). */
constexpr double KAPPA = 0.40;
/** @brief Radius of the earth (m). */
constexpr double RADIUS = 4.0e7 / TPI;

// Derived physical constants
/** @brief Inverse of gravity^2 * (2*Pi)^3. */
constexpr double G2PI3I = 1.0 / (GRAV * GRAV * TPI * TPI * TPI);
/** @brief Inverse of gravity * 2 * Pi. */
constexpr double G1PI1I = 1.0 / (GRAV * TPI);

// Model constants
/** @brief Undefined value. */
constexpr double UNDEF = -999.9;
/** @brief Minimum value for spectral density (log10). */
constexpr double ABMIN = -1.0;
/** @brief Maximum value for spectral density (log10). */
constexpr double ABMAX = 8.0;

} // namespace constants

/**
 * @namespace ww4_service
 * @brief Namespace providing utility mathematical routines.
 * @details This namespace contains ported routines from WAVEWATCH III (WW3).
 */
namespace ww4_service {

/**
 * @brief Calculate 5-parameter JONSWAP spectrum.
 * @details Ported from WW3 routine EJ5P in w3srcemd.f90.
 *          Original author in WW3: G. Ph. van Vledder.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-10
 * @param f Frequency (Hz).
 * @param fp Peak frequency (Hz).
 * @param alpha Phillip's constant.
 * @param gamma Peak enhancement factor.
 * @param siga Sigma_a (for f <= fp).
 * @param sigb Sigma_b (for f > fp).
 * @return Spectral density E(f).
 */
double JONSWAP_5p(double f, double fp, double alpha, double gamma, double siga,
                  double sigb);

/**
 * @brief Calculate the haversine distance between two points on a sphere.
 * @details Ported from WW3 routine DIST_HAVERSINE in w3servmd.F90.
 *          Method: R.W. Sinnott, "Virtues of the Haversine",
 *                  Sky and Telescope, vol. 68, no. 2, 1984, p. 159.
 *          Original author in WW3: D. A. Honegger (USACE/ERDC).
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-10
 * @param lon1 Longitude of 1st point (degrees).
 * @param lat1 Latitude of 1st point (degrees).
 * @param lon2 Longitude of 2nd point (degrees).
 * @param lat2 Latitude of 2nd point (degrees).
 * @return Spherical distance (radians).
 */
double dist_Haversine(double lon1, double lat1, double lon2, double lat2);

/**
 * @brief Calculate the spherical distance between two points in meters.
 * @details Ported from WW3 routine DIST_SPHERE in w3srcemd.f90.
 *          Uses the haversine formula for computation.
 *          Original author in WW3: Fabrice Ardhuin.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-04-10
 * @param lon1 Longitude of 1st point (degrees).
 * @param lat1 Latitude of 1st point (degrees).
 * @param lon2 Longitude of 2nd point (degrees).
 * @param lat2 Latitude of 2nd point (degrees).
 * @return Spherical distance (meters).
 */
double dist_on_sphere(double lon1, double lat1, double lon2, double lat2);

} // namespace ww4_service

} // namespace ww4_utils

#endif // WW4_UTILS_WW4_SERVICE_HPP
