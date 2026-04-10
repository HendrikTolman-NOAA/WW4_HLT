/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file ww4_service.hpp
 * @brief Mathematical, physical, and model constants and service routines.
 * @details This header defines the ww4Service class, providing core constants
 *          and utility routines ported from WAVEWATCH III.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @date Initial, 2026-04-09
 * @date Last Update, 2026-04-10
 *
 * @note This file is converted from WAVEWATCH III (WW3) source files
 *       constants.F90 and w3servmd.F90.
 */

#ifndef WW4_UTILS_WW4_SERVICE_HPP
#define WW4_UTILS_WW4_SERVICE_HPP

namespace ww4_utils {

/**
 * @class ww4Service
 * @brief Service utility class containing constants and general routines.
 * @details Ported from WAVEWATCH III constants.F90 and w3servmd.F90.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 */
class ww4Service {
public:
  // Mathematical constants
  static constexpr double PI = 3.14159265358979323846;
  static constexpr double TPI = 2.0 * PI;
  static constexpr double HPI = 0.5 * PI;
  static constexpr double TPIINV = 1.0 / TPI;
  static constexpr double HPIINV = 1.0 / HPI;
  static constexpr double RADE = 180.0 / PI;
  static constexpr double DERA = PI / 180.0;

  // Physical constants
  static constexpr double GRAV = 9.806;
  static constexpr double DWAT = 1000.0;
  static constexpr double DAIR = 1.225;
  static constexpr double NU_AIR = 1.4E-5;
  static constexpr double NU_WATER = 1.31E-6;
  static constexpr double SED_SG = 2.65;
  static constexpr double KAPPA = 0.40;
  static constexpr double RADIUS = 4.0E7 * TPIINV;

  // Model constants
  static constexpr double G2PI3I = 1.0 / (GRAV * GRAV * TPI * TPI * TPI);
  static constexpr double G1PI1I = 1.0 / (GRAV * TPI);
  static constexpr double UNDEF = -999.9;
  static constexpr double ABMIN = -1.0;

  /**
   * @brief Computation of spectral density using a 5-parameter
   * JONSWAP-spectrum.
   * @details Converted from WW3 routine EJ5P.
   *          JONSWAP_5p(F) = A * EXP(B + LN(Y) * EXP(C))
   * @note Original author in WW3: G. Ph. van Vledder.
   * @param f Frequency in Hz.
   * @param alfa Energy scaling factor.
   * @param fp Peak frequency in Hz.
   * @param yln Peak overshoot factor, given by LN-value.
   * @param siga Spectral width, for f < fp.
   * @param sigb Spectral width, for f > fp.
   * @return Spectral density.
   */
  static double JONSWAP_5p(double f, double alfa, double fp, double yln,
                           double siga, double sigb) noexcept;
};

} // namespace ww4_utils

#endif // WW4_UTILS_WW4_SERVICE_HPP
