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
 * @date Initial, 2026-04-09
 */

#ifndef WW4_UTILS_WW4_SERVICE_HPP
#define WW4_UTILS_WW4_SERVICE_HPP

namespace ww4_utils {

/**
 * @namespace constants
 * @brief Namespace containing all physical and mathematical constants.
 * @details This namespace contains constants converted from the WAVEWATCH III
 *          (WW3) source files constants.F90 and mod_constants.f90.
 *          Original author in WW3: Hendrik L. Tolman.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
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
/** @brief Square root of 2. */
constexpr double SQRT2 = 1.414213562373095;

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
/** @brief Square of gravity. */
constexpr double GSQ = GRAV * GRAV;

// Model constants
/** @brief Value for undefined variable in output. */
constexpr double UNDEF = -999.9;
/** @brief Minimum value for friction factor table argument. */
constexpr double ABMIN = -1.0;
/** @brief Maximum value for friction factor table argument. */
constexpr double ABMAX = 8.0;
/** @brief Minimum argument for exp function to avoid underflow. */
constexpr double EXPMIN = -20.0;
/** @brief Maximum argument for exp function to avoid overflow. */
constexpr double EXPMAX = 20.0;

/** @brief External constants defined in source file. */
extern const double SQRTG;

} // namespace constants

} // namespace ww4_utils

#endif // WW4_UTILS_WW4_SERVICE_HPP
