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
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Rahul Mahajan
 * @date 2026-05-01
 */

#pragma once

namespace ww4_constants {

// Mathematical constants
constexpr double PI = 3.14159265358979323846;
constexpr double TPI = 2.0 * PI;
constexpr double HPI = 0.5 * PI;
constexpr double TPIINV = 1.0 / TPI;
constexpr double HPIINV = 1.0 / HPI;
constexpr double RADE = 180.0 / PI;
constexpr double DERA = PI / 180.0;

// Physical constants
constexpr double GRAV = 9.806;
constexpr double DWAT = 1000.0;
constexpr double DAIR = 1.225;
constexpr double NU_AIR = 1.4e-5;
constexpr double NU_WATER = 1.31e-6;
constexpr double SED_SG = 2.65;
constexpr double KAPPA = 0.40;
constexpr double RADIUS = 4.0e7 / TPI;

// Derived physical constants
constexpr double G2PI3I = 1.0 / (GRAV * GRAV * TPI * TPI * TPI);
constexpr double G1PI1I = 1.0 / (GRAV * TPI);

// Model constants
constexpr double UNDEF = -999.9;
constexpr double ABMIN = -1.0;
constexpr double ABMAX = 8.0;

constexpr double KDMAX = 20.0;

constexpr double JONSWAP_FACTOR = 0.06175;

} // namespace ww4_constants
