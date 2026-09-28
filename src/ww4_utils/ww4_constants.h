/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_constants.h
 * @brief Common mathematical and physical constants for WAVEWATCH IV.
 * @details This header defines a set of shared constants used across the WW4
 *          model, moved from ww4_service.h.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Rahul Mahajan
 * @date Initial, 2026-05-21
 * @date Last update : 2026-09-24
 */

#pragma once

#include <numbers>

namespace ww4_constants {

// --- Mathematical constants -------------------------------------------------
/** Conversion factor from radians to degrees. */
constexpr double Radians2Degrees = 180.0 / std::numbers::pi;
/** Conversion factor from degrees to radians. */
constexpr double Degrees2Radians = std::numbers::pi / 180.0;

// --- Physical constants -----------------------------------------------------
/** Acceleration of gravity (m/s^2). */
constexpr double GRAV = 9.806;
/** Density of water (kg/m^3). */
constexpr double DWAT = 1000.0;
/** Density of air (kg/m^3). */
constexpr double DAIR = 1.225;
/** Kinematic viscosity of air (m^2/s). */
constexpr double NU_AIR = 1.4e-5;
/** Kinematic viscosity of water (m^2/s). */
constexpr double NU_WATER = 1.31e-6;
/** Specific gravity of sediments (dimensionless). */
constexpr double SED_SG = 2.65;
/** von Karman's constant (dimensionless). */
constexpr double KAPPA = 0.40;
/** Mean radius of the earth (m). */
constexpr double RADIUS = 4.0e7 / (2.0 * std::numbers::pi);

// --- Model constants --------------------------------------------------------
/** Undefined numerical value indicator. */
constexpr double UNDEF = -999.9;
/** Minimum value for spectral density (log10). */
constexpr double ABMIN = -1.0;
/** Maximum value for spectral density (log10). */
constexpr double ABMAX = 8.0;
/** Maximum value for k*d in dispersion calculations. */
constexpr double KDMAX = 20.0;
/** Physics factor for JONSWAP spectrum. */
constexpr double JONSWAP_FACTOR = 0.06175;

} // namespace ww4_constants
