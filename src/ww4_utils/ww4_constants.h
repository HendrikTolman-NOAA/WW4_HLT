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
 * NWS often uses Generative AI (GenAI) for code development and refactoring. Whenever GenAI is used, NWS requires a full human review of code before it is added to its repositories
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Rahul Mahajan
 * @date 2026-05-21
 */

#pragma once

#include <numbers>

namespace ww4_constants {

// Mathematical constants
constexpr double Radians2Degrees = 180.0 / std::numbers::pi;
constexpr double Degrees2Radians = std::numbers::pi / 180.0;

// Physical constants
constexpr double GRAV = 9.806;
constexpr double DWAT = 1000.0;
constexpr double DAIR = 1.225;
constexpr double NU_AIR = 1.4e-5;
constexpr double NU_WATER = 1.31e-6;
constexpr double SED_SG = 2.65;
constexpr double KAPPA = 0.40;
constexpr double RADIUS = 4.0e7 / (2.0 * std::numbers::pi);

// Model constants
constexpr double UNDEF = -999.9;
constexpr double ABMIN = -1.0;
constexpr double ABMAX = 8.0;

constexpr double KDMAX = 20.0;

constexpr double JONSWAP_FACTOR = 0.06175;

} // namespace ww4_constants
